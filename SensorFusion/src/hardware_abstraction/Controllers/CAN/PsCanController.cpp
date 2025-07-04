#include <hardware_abstraction/Controllers/CAN/CanCommandsRegisters.h>
#include <hardware_abstraction/Controllers/CAN/PsCanController.h>
#include "services/Exception/SystemExceptions.h"
#include "services/Logger/LoggerMacros.h"

#define CANPS_IRQ

#ifdef CANPS_IRQ
#include "xinterrupt_wrap.h"
#endif
#include <math.h>

extern XScuGic xInterruptController;
static constexpr uint8_t RESET_CONTROLLER_COUNTER = 128;

namespace hardware_abstraction
{
namespace Controllers
{

constexpr uint8_t READ_STATUS_REG_RETRIES = 10;

PsCanController::PsCanController() : m_deviceId(XPAR_XCANPS_0_DEVICE_ID), m_initialized(false)
{
	m_timingConfiguration = canTimingPresets[CanBusBaudrates::_250kbps];
	canMutex = std::make_shared<business_logic::Osal::MutexHandler>();
}

PsCanController::~PsCanController()
{

}

void PsCanController::initialize()
{
	if(!m_initialized)
	{

		m_config = XCanPs_LookupConfig(m_deviceId);
		XCanPs_CfgInitialize(&m_canPs, m_config, m_config->BaseAddr);
		/*
		 * Run self-test on the device, which verifies basic sanity of the
		 * device and the driver.
		 */
		auto status = XCanPs_SelfTest(&m_canPs);
		if (status != XST_SUCCESS)
		{
			THROW_CONTROLLERS_EXCEPTION(services::ControllersErrorId::CanInitializationError, "CanController error during XCanPs_SelfTest");
		}

		/*
		 * Enter Configuration Mode if the device is not currently in
		 * Configuration Mode.
		 */
		configureTransceiver(CanPsMode::Config);
		/*
		 * Setup Baud Rate Prescaler Register (BRPR) and
		 * Bit Timing Register (BTR).
		 */
		XCanPs_SetBaudRatePrescaler(&m_canPs, m_timingConfiguration.brpr);
		XCanPs_SetBitTiming(&m_canPs, m_timingConfiguration.sjw, m_timingConfiguration.tseg2, m_timingConfiguration.tseg1);

#ifdef CANPS_IRQ
		/*
		 * Set interrupt handlers.
		 */

		XCanPs_SetHandler(&m_canPs, XCANPS_HANDLER_SEND, sendHandler, this);
		XCanPs_SetHandler(&m_canPs, XCANPS_HANDLER_RECV, recvHandler, this);
		XCanPs_SetHandler(&m_canPs, XCANPS_HANDLER_ERROR, errorHandler, this);
		XCanPs_SetHandler(&m_canPs, XCANPS_HANDLER_EVENT, eventHandler, this);

		status = XScuGic_Connect(&xInterruptController, XPAR_XCANPS_0_INTR, (Xil_InterruptHandler)XCanPs_IntrHandler, (void *)&m_canPs);
		if (status != XST_SUCCESS)
		{
			LOG_ERROR("Connection Can IRQ to GIC");
		}

		/*
		 * Enable the interrupt for the CAN device.
		 */
		XScuGic_Enable(&xInterruptController, XPAR_XCANPS_0_INTR);
		/*
		 * Initialize the flags.
		 */
		m_sendDone = false;
		m_recvDone = false;

		/*
		 * Enable all interrupts in CAN device.
		 */
		XCanPs_IntrEnable(&m_canPs, XCANPS_IXR_ALL);

		//auto isTestOk = selfTest();

//		if(status != XST_SUCCESS)
//		{
//			THROW_CONTROLLERS_EXCEPTION(services::ControllersErrorId::CanInitializationError, "CanController error during initialization");
//		}

#endif
		configureTransceiver(CanPsMode::NormalOperation);
		m_initialized = true;
	}
	else
	{
		LOG_WARNING("CanController already initialized");
	}
}

void PsCanController::configureTransceiver(CanPsMode mode)
{
	uint8_t modeRegisterValue;
	switch (mode) {
		case CanPsMode::Config:
			modeRegisterValue = XCANPS_MODE_CONFIG;
			break;
		case CanPsMode::NormalOperation:
			modeRegisterValue = XCANPS_MODE_NORMAL;
			break;
		case CanPsMode::Loopback:
			modeRegisterValue = XCANPS_MODE_LOOPBACK;
			break;
		case CanPsMode::Sleep:
			modeRegisterValue = XCANPS_MODE_SLEEP;
			break;
		case CanPsMode::Snoop:
			modeRegisterValue = XCANPS_MODE_SNOOP;
			break;
		default:
			break;
	}
	uint8_t waitForModeCnt = 0;
	XCanPs_EnterMode(&m_canPs, modeRegisterValue);
	while (XCanPs_GetMode(&m_canPs) != modeRegisterValue)
	{
		if(waitForModeCnt > 3)
		{
			//TODO rise error
			THROW_CONTROLLERS_EXCEPTION(services::ControllersErrorId::CanConfigTransceiverError, "CanController error during XCanPs_EnterMode");
		}
		usleep(100);
		waitForModeCnt++;
	}
}

void PsCanController::modifyRegister(uint8_t reg, uint8_t mask, uint8_t value)
{

}


void PsCanController::writeRegister(uint8_t reg, uint8_t *data, uint32_t nData)
{

}

void PsCanController::clearRegister(uint8_t reg, uint32_t nData)
{

}

bool PsCanController::transmitMsg(uint8_t idMsg, uint8_t *txMsg, uint8_t msgLength)
{
	CanFrame message;

	message.id = idMsg;
	message.dlc = msgLength;
	message.eid = 0x15a;
	message.rtr = 0;
	message.ide = 0;
	for(int i = 0; i<msgLength; i++)
	{
		message.data[i] = txMsg[i];
	}

	canMutex->lock();

	uint8_t waitStatusRegisterRetries = 0;
	while (XCanPs_IsTxFifoFull(&m_canPs) == TRUE)
	{
		waitStatusRegisterRetries++;
		if(waitStatusRegisterRetries > READ_STATUS_REG_RETRIES)
		{
			LOG_WARNING("Spi status register is not ready");
			canMutex->unlock();
			return false;
		}
		usleep(100);
	}

	transmit(message);

	canMutex->unlock();
	return true;
}

void PsCanController::transmit(CanFrame msg)
{
	u8 *framePtr;

	/*
	 * Create correct values for Identifier and Data Length Code Register.
	 */
	m_txFrame[0] = static_cast<uint32_t>(XCanPs_CreateIdValue(static_cast<uint32_t>(msg.id), 0, 0, 0, 0));
	m_txFrame[1] = static_cast<uint32_t>(XCanPs_CreateDlcValue(static_cast<uint32_t>(msg.dlc)));


	framePtr = (u8 *)(&m_txFrame[2]);
	for (uint8_t index = 0; index < msg.dlc; index++)
	{
		*framePtr++ = static_cast<uint8_t>(msg.data[index]);
	}

	/*
	 * Now wait until the TX FIFO is not full and send the frame.
	 */
	while (XCanPs_IsTxFifoFull(&m_canPs) == TRUE);

	auto status = XCanPs_Send(&m_canPs, m_txFrame);
	if (status != XST_SUCCESS)
	{
#ifdef CANPS_IRQ

		m_sendDone = true;

#endif
	}
}

std::vector<CanFrame> PsCanController::receiveMsg(bool& assembleFrame, bool& endOfImage)
{
	LOG_TRACE("PsCanController::receiveMsg ulTaskNotifyTake");
	ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
	LOG_TRACE("PsCanController::receiveMsg ulTaskNotifyTake done");
	//canMutex->lock();
	//while(m_recvDone == false);
	assembleFrame = m_recvDone;
	endOfImage = m_endOfImage;
	m_recvDone = false;
	//canMutex->unlock();
	return m_rxMsgVector;
}

void PsCanController::clearBuffer()
{
	m_rxMsgVector.clear();
}

bool PsCanController::selfTest()
{

//	/*
//	 * Enter Loop Back Mode.
//	 */
//	configureTransceiver(CanPsMode::Loopback);
//
//	/*
//	 * Loop back a frame. The RecvHandler is expected to handle
//	 * the frame reception.
//	 */
//	uint8_t idMsg = 0x03;
//	uint8_t txMsg[5] = {0x1,0x2,0x3,0x4,0x44};
//	uint8_t msgLength = 8;
//	transmitMsg(idMsg, txMsg, msgLength); /* Send a frame */
//
//	/*
//	 * Wait here until both sending and reception have been completed.
//	 */
//#ifdef CANPS_IRQ
//	while (m_sendDone != true);
//#else
//	usleep(500000);
//#endif
//	auto rxMsg = receiveMsg();
//
//	return true;
}

void PsCanController::resetController()
{
	//LOG_ERROR("PsCanController::resetController");
	XCanPs_Reset(&m_canPs);
	m_initialized = false;
	initialize();
}
/*
 * AVOID CALL THIS METHOD BECAUSE STOP SCHEDULER!!!
 */
int PsCanController::setupInterruptSystem(XScuGic *IntcInstancePtr, XCanPs *CanInstancePtr, u16 CanIntrId)
{
	int Status;
	return;

	XScuGic_Config *IntcConfig; /* Instance of the interrupt controller */

	Xil_ExceptionInit();

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
				       IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,(Xil_ExceptionHandler)XScuGic_InterruptHandler,IntcInstancePtr);


	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	Status = XScuGic_Connect(IntcInstancePtr, CanIntrId, (Xil_InterruptHandler)XCanPs_IntrHandler, (void *)CanInstancePtr);
	if (Status != XST_SUCCESS)
	{
		return Status;
	}

	/*
	 * Enable the interrupt for the CAN device.
	 */
	XScuGic_Enable(IntcInstancePtr, CanIntrId);

	/*
	 * Enable interrupts in the Processor.
	 */
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}
  
void PsCanController::registerTaskToNotify(const TaskHandle_t& xReceiveTaskToNotify)
{
	m_receiveTaskToNotify = xReceiveTaskToNotify;
}

#ifdef CANPS_IRQ
static void PsCanController::sendHandler(void *CallBackRef)
{
	PsCanController *self = static_cast<PsCanController *>(CallBackRef);
	self->m_errorCounter = 0;
	/*
	 * The frame was sent successfully. Notify the task context.
	 */
	//LOG_DEBUG("SendHandler executed");
	//std::cout << "SendHandler executing" << std::endl;
	//std::cout << "SendHandler done" << std::endl;
	//self->m_sendDone = true;
}


static void PsCanController::recvHandler(void *CallBackRef)
{
	//LOG_DEBUG("RecvHandler executed");
	//std::cout << "RecvHandler executing" << std::endl;

	CanFrame rxMsg;
	uint8_t* framePtr;
	PsCanController *self = static_cast<PsCanController *>(CallBackRef);
	memset(self->m_rxFrame, 0, sizeof(self->m_rxFrame));
	self->m_endOfImage = false;
	self->m_errorCounter = 0;
	auto status = XCanPs_Recv(&self->m_canPs, self->m_rxFrame);
	if (status == XST_SUCCESS)
	{
		rxMsg.id  = (self->m_rxFrame[0] & XCANPS_IDR_ID1_MASK) >> XCANPS_IDR_ID1_SHIFT;
		rxMsg.dlc = (self->m_rxFrame[1] & XCANPS_DLCR_DLC_MASK) >> XCANPS_DLCR_DLC_SHIFT;

		framePtr = (uint8_t *)(&self->m_rxFrame[2]);
		for (auto idx = 0; idx < rxMsg.dlc; idx++)
		{
			rxMsg.data[idx] = *framePtr++;
		}

		if(rxMsg.data[1] == 0x1E && rxMsg.data[2] == 0x1E && rxMsg.data[3] == 0x1E &&
				rxMsg.data[4] == 0x1E && rxMsg.data[5] == 0x1E && rxMsg.data[6] == 0x1E)
		{
			//LOG_DEBUG("PsCanController::recvHandler EoF received");
			m_endOfImage = rxMsg.data[7];
			//std::cout << "PsCanController::recvHandler EoF received with: " << std::to_string(self->m_rxMsgVector.size()) << " CAN frames -- EndOfImage "<< std::to_string(m_endOfImage) <<std::endl;
			self->m_recvDone = true;
			//std::cout << "PsCanController::recvHandler vTaskNotifyGiveIndexedFromISR" << std::endl;
			BaseType_t xHigherPriorityTaskWoken = pdFALSE;
			vTaskNotifyGiveIndexedFromISR( m_receiveTaskToNotify, 0, &xHigherPriorityTaskWoken );
			//std::cout << "PsCanController::recvHandler vTaskNotifyGiveIndexedFromISR done" << std::endl;
		}
		else
		{
			self->m_rxMsgVector.push_back(rxMsg);
			//std::cout << "PsCanController::recvHandler normal frame: " /*<< std::to_string(self->m_rxMsgVector.size()) */<< std::endl;
		}
	}
}

static void PsCanController::errorHandler(void *CallBackRef, u32 ErrorMask)
{
	PsCanController *self = static_cast<PsCanController *>(CallBackRef);
	self->m_errorCounter++;
	if (ErrorMask & XCANPS_ESR_ACKER_MASK) {
		/*
		 * ACK Error handling code should be put here.
		 */
		//LOG_DEBUG("ErrorHandler XCANPS_ESR_ACKER_MASK");
		std::cout << "PsCanController ACK Error" << std::endl;
	}

	if (ErrorMask & XCANPS_ESR_BERR_MASK) {
		/*
		 * Bit Error handling code should be put here.
		 */
		//LOG_DEBUG("ErrorHandler XCANPS_ESR_BERR_MASK");
		std::cout << "PsCanController Bit Error" << std::endl;
	}

	if (ErrorMask & XCANPS_ESR_STER_MASK) {
		/*
		 * Stuff Error handling code should be put here.
		 */
		//LOG_DEBUG("ErrorHandler XCANPS_ESR_STER_MASK");
		std::cout << "PsCanController Stuff Error" << std::endl;
	}

	if (ErrorMask & XCANPS_ESR_FMER_MASK) {
		/*
		 * Form Error handling code should be put here.
		 */
		//LOG_DEBUG("ErrorHandler XCANPS_ESR_FMER_MASK");
		//std::cout << "PsCanController Form Error" << std::endl;
	}

	if (ErrorMask & XCANPS_ESR_CRCER_MASK) {
		/*
		 * CRC Error handling code should be put here.
		 */
		//LOG_DEBUG("ErrorHandler XCANPS_ESR_CRCER_MASK");
		//std::cout << "PsCanController CRC Error" << std::endl;
	}
	if(self->m_errorCounter > RESET_CONTROLLER_COUNTER)
	{
//		self->m_errorCounter = 0;
//		self->resetController();
		std::cout << "PsCanController Reset PSCan Controller" << std::endl;
	}
//	const auto errorBusStatus = XCanPs_GetBusErrorStatus(&self->m_canPs);
//	u8 rxErrorCount = 0, txErrorCount = 0;
//	XCanPs_GetBusErrorCounter(&self->m_canPs, &rxErrorCount, &txErrorCount);
//	xil_printf("errorBusStatus: %d\n", errorBusStatus);
//	xil_printf("RX Error Counter: %d\n", rxErrorCount);
//	xil_printf("TX Error Counter: %d\n", txErrorCount);
//	if(rxErrorCount>20 || txErrorCount>20)
//	{
//		std::cout << "PsCanController::eventHandler Entering Bus off status interrupt" << std::endl;
//		self->resetController();
//	}
}

static void PsCanController::eventHandler(void *CallBackRef, u32 IntrMask)
{
	PsCanController *self = static_cast<PsCanController *>(CallBackRef);
	std::cout << "PsCanController::eventHandler triggered" << std::endl;
	if (IntrMask & XCANPS_IXR_BSOFF_MASK) {
		/*
		 * Entering Bus off status interrupt requires
		 * the CAN device be reset and reconfigured.
		 */
		//LOG_DEBUG("EventHandler XCANPS_IXR_BSOFF_MASK");
		std::cout << "PsCanController::eventHandler Entering Bus off status interrupt" << std::endl;
		//self->resetController();
		//return;
	}

	if (IntrMask & XCANPS_IXR_RXOFLW_MASK) {
		/*
		 * Code to handle RX FIFO Overflow Interrupt should be put here.
		 */
		//LOG_DEBUG("EventHandler XCANPS_IXR_RXOFLW_MASK");
		std::cout << "PsCanController::eventHandler RX FIFO Overflow" << std::endl;
	}

	if (IntrMask & XCANPS_IXR_RXUFLW_MASK) {
		/*
		 * Code to handle RX FIFO Underflow Interrupt
		 * should be put here.
		 */
		//LOG_DEBUG("EventHandler XCANPS_IXR_RXUFLW_MASK");
		std::cout << "PsCanController::eventHandler RX FIFO Underflow" << std::endl;
	}

	if (IntrMask & XCANPS_IXR_TXBFLL_MASK) {
		/*
		 * Code to handle TX High Priority Buffer Full
		 * Interrupt should be put here.
		 */
		//LOG_DEBUG("EventHandler XCANPS_IXR_TXBFLL_MASK");
		std::cout << "PsCanController::eventHandler TX High Priority Buffer Full" << std::endl;
	}

	if (IntrMask & XCANPS_IXR_TXFLL_MASK) {
		/*
		 * Code to handle TX FIFO Full Interrupt should be put here.
		 */
		//LOG_DEBUG("EventHandler XCANPS_IXR_TXFLL_MASK");
		std::cout << "PsCanController::eventHandler TX FIFO Full Interrupt" << std::endl;
	}

	if (IntrMask & XCANPS_IXR_WKUP_MASK) {
		/*
		 * Code to handle Wake up from sleep mode Interrupt
		 * should be put here.
		 */
		//LOG_DEBUG("EventHandler XCANPS_IXR_WKUP_MASK");
		std::cout << "PsCanController::eventHandler Wake up from sleep mode" << std::endl;
	}

	if (IntrMask & XCANPS_IXR_SLP_MASK) {
		/*
		 * Code to handle Enter sleep mode Interrupt should be put here.
		 */
		//LOG_DEBUG("EventHandler XCANPS_IXR_SLP_MASK");
		std::cout << "XCANPS_IXR_SLP_MASK executing" << std::endl;
		std::cout << "PsCanController::eventHandler Enter to sleep mode" << std::endl;
	}

	if (IntrMask & XCANPS_IXR_ARBLST_MASK) {
		/*
		 * Code to handle Lost bus arbitration Interrupt
		 * should be put here.
		 */
		//LOG_DEBUG("EventHandler XCANPS_IXR_ARBLST_MASK");
		std::cout << "PsCanController::eventHandler Lost bus arbitration" << std::endl;
	}
}

#endif

}
}
