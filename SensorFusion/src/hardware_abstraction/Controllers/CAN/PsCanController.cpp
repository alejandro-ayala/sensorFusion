#include <hardware_abstraction/Controllers/CAN/CanCommandsRegisters.h>
#include <hardware_abstraction/Controllers/CAN/PsCanController.h>
#include "services/Exception/SystemExceptions.h"
#include "services/Logger/LoggerMacros.h"
#include "xinterrupt_wrap.h"

namespace hardware_abstraction
{
namespace Controllers
{

#define CAN_DEVICE_ID		XPAR_XCANPS_0_DEVICE_ID
#define CAN_INTR_VEC_ID		XPAR_INTC_0_CANPS_0_VEC_ID
/*
 * Timing parameters to be set in the Bit Timing Register (BTR).
 * These values are for a 40 Kbps baudrate assuming the CAN input clock
 * frequency is 24 MHz.
 */
#define TEST_BTR_SYNCJUMPWIDTH		3
#define TEST_BTR_SECOND_TIMESEGMENT	2
#define TEST_BTR_FIRST_TIMESEGMENT	15

/*
 * The Baud rate Prescalar value in the Baud Rate Prescaler Register
 * needs to be set based on the input clock  frequency to the CAN core and
 * the desired CAN baud rate.
 * This value is for a 40 Kbps baudrate assuming the CAN input clock frequency
 * is 24 MHz.
 */
#define TEST_BRPR_BAUD_PRESCALAR	29
constexpr uint8_t READ_STATUS_REG_RETRIES = 10;

static void SendHandler(void *CallBackRef);
static void RecvHandler(void *CallBackRef);
static void ErrorHandler(void *CallBackRef, u32 ErrorMask);
static void EventHandler(void *CallBackRef, u32 Mask);
/*
 * Shared variables used to test the callbacks.
 */
static volatile int LoopbackError;	/* Asynchronous error occurred */
static volatile int RecvDone;		/* Received a frame */
static volatile int SendDone;		/* Frame was sent successfully */
/*****************************************************************************/
/**
*
* Callback function (called from interrupt handler) to handle confirmation of
* transmit events when in interrupt mode.
*
* @param	CallBackRef is the callback reference passed from the interrupt
*		handler, which in our case is a pointer to the driver instance.
*
* @return	None.
*
* @note		This function is called by the driver within interrupt context.
*
******************************************************************************/
static void SendHandler(void *CallBackRef)
{
	/*
	 * The frame was sent successfully. Notify the task context.
	 */
	(void)CallBackRef;
	SendDone = TRUE;
}


/*****************************************************************************/
/**
*
* Callback function (called from interrupt handler) to handle frames received in
* interrupt mode.  This function is called once per frame received.
* The driver's receive function is called to read the frame from RX FIFO.
*
* @param	CallBackRef is the callback reference passed from the interrupt
*		handler, which in our case is a pointer to the device instance.
*
* @return	None.
*
* @note		This function is called by the driver within interrupt context.
*
******************************************************************************/
static void RecvHandler(void *CallBackRef)
{
	RecvDone = TRUE;
}


/*****************************************************************************/
/**
*
* Callback function (called from interrupt handler) to handle error interrupt.
* Error code read from Error Status register is passed into this function.
*
* @param	CallBackRef is the callback reference passed from the interrupt
*		handler, which in our case is a pointer to the driver instance.
* @param	ErrorMask is a bit mask indicating the cause of the error.
*		Its value equals 'OR'ing one or more XCANPS_ESR_* defined in
*		xcanps_hw.h.
*
* @return	None.
*
* @note		This function is called by the driver within interrupt context.
*
******************************************************************************/
static void ErrorHandler(void *CallBackRef, u32 ErrorMask)
{
	(void)CallBackRef;
	if (ErrorMask & XCANPS_ESR_ACKER_MASK) {
		/*
		 * ACK Error handling code should be put here.
		 */
	}

	if (ErrorMask & XCANPS_ESR_BERR_MASK) {
		/*
		 * Bit Error handling code should be put here.
		 */
	}

	if (ErrorMask & XCANPS_ESR_STER_MASK) {
		/*
		 * Stuff Error handling code should be put here.
		 */
	}

	if (ErrorMask & XCANPS_ESR_FMER_MASK) {
		/*
		 * Form Error handling code should be put here.
		 */
	}

	if (ErrorMask & XCANPS_ESR_CRCER_MASK) {
		/*
		 * CRC Error handling code should be put here.
		 */
	}

	/*
	 * Set the shared variables.
	 */
	LoopbackError = TRUE;
	RecvDone = TRUE;
	SendDone = TRUE;
}

static void EventHandler(void *CallBackRef, u32 IntrMask)
{
	XCanPs *CanPtr = (XCanPs *)CallBackRef;

	if (IntrMask & XCANPS_IXR_BSOFF_MASK) {
		/*
		 * Entering Bus off status interrupt requires
		 * the CAN device be reset and reconfigured.
		 */
		XCanPs_Reset(CanPtr);
		return;
	}

	if (IntrMask & XCANPS_IXR_RXOFLW_MASK) {
		/*
		 * Code to handle RX FIFO Overflow Interrupt should be put here.
		 */
	}

	if (IntrMask & XCANPS_IXR_RXUFLW_MASK) {
		/*
		 * Code to handle RX FIFO Underflow Interrupt
		 * should be put here.
		 */
	}

	if (IntrMask & XCANPS_IXR_TXBFLL_MASK) {
		/*
		 * Code to handle TX High Priority Buffer Full
		 * Interrupt should be put here.
		 */
	}

	if (IntrMask & XCANPS_IXR_TXFLL_MASK) {
		/*
		 * Code to handle TX FIFO Full Interrupt should be put here.
		 */
	}

	if (IntrMask & XCANPS_IXR_WKUP_MASK) {
		/*
		 * Code to handle Wake up from sleep mode Interrupt
		 * should be put here.
		 */
	}

	if (IntrMask & XCANPS_IXR_SLP_MASK) {
		/*
		 * Code to handle Enter sleep mode Interrupt should be put here.
		 */
	}

	if (IntrMask & XCANPS_IXR_ARBLST_MASK) {
		/*
		 * Code to handle Lost bus arbitration Interrupt
		 * should be put here.
		 */
	}
}

PsCanController::PsCanController() : m_deviceId(XPAR_XCANPS_0_DEVICE_ID), m_initialized(false)
{
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
		XCanPs_SetBaudRatePrescaler(&m_canPs, TEST_BRPR_BAUD_PRESCALAR);
		XCanPs_SetBitTiming(&m_canPs, TEST_BTR_SYNCJUMPWIDTH, TEST_BTR_SECOND_TIMESEGMENT, TEST_BTR_FIRST_TIMESEGMENT);

		/*
		 * Set interrupt handlers.
		 */
		XCanPs_SetHandler(&m_canPs, XCANPS_HANDLER_SEND, (void *)SendHandler, (void *)&m_canPs);
		XCanPs_SetHandler(&m_canPs, XCANPS_HANDLER_RECV, (void *)RecvHandler, (void *)&m_canPs);
		XCanPs_SetHandler(&m_canPs, XCANPS_HANDLER_ERROR, (void *)ErrorHandler, (void *)&m_canPs);
		XCanPs_SetHandler(&m_canPs, XCANPS_HANDLER_EVENT, (void *)EventHandler, (void *)&m_canPs);
		/*
		 * Initialize the flags.
		 */
		SendDone = FALSE;
		RecvDone = FALSE;
		LoopbackError = FALSE;

		status =  setupInterruptSystem(&m_irqController, &m_canPs, XPAR_XCANPS_0_INTR);
		if (status != XST_SUCCESS)
		{
			THROW_CONTROLLERS_EXCEPTION(services::ControllersErrorId::CanInitializationError, "CanController error during setupInterruptSystem");
		}
		/*
		 * Enable all interrupts in CAN device.
		 */
		XCanPs_IntrEnable(&m_canPs, XCANPS_IXR_ALL);

		//auto isTestOk = selfTest();
		if(status != XST_SUCCESS)
		{
			THROW_CONTROLLERS_EXCEPTION(services::ControllersErrorId::CanInitializationError, "CanController error during initialization");
		}
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

		LoopbackError = TRUE;
		SendDone = TRUE;
		RecvDone = TRUE;
	}
}

CanFrame PsCanController::receiveMsg()
{

	CanFrame rxMsg;
	uint8_t* framePtr;
	auto status = XCanPs_Recv(&m_canPs, m_rxFrame);
	if (status == XST_SUCCESS)
	{
		rxMsg.id  = (m_rxFrame[0] & XCANPS_IDR_ID1_MASK) >> XCANPS_IDR_ID1_SHIFT;
		rxMsg.dlc = (m_rxFrame[1] & XCANPS_DLCR_DLC_MASK) >> XCANPS_DLCR_DLC_SHIFT;

		framePtr = (uint8_t *)(&m_rxFrame[2]);
		for (auto idx = 0; idx < rxMsg.dlc; idx++)
		{
			rxMsg.data[idx] = *framePtr++;
		}
		RecvDone = TRUE;
	}
	else
	{
		rxMsg.dlc = 0;
	}

	return rxMsg;
}

bool PsCanController::selfTest()
{

	/*
	 * Enter Loop Back Mode.
	 */
	configureTransceiver(CanPsMode::Loopback);

	/*
	 * Loop back a frame. The RecvHandler is expected to handle
	 * the frame reception.
	 */
	uint8_t idMsg = 0x03;
	uint8_t txMsg[5] = {0x1,0x2,0x3,0x4,0x44};
	uint8_t msgLength = 8;
	transmitMsg(idMsg, txMsg, msgLength); /* Send a frame */

	/*
	 * Wait here until both sending and reception have been completed.
	 */
	while ((SendDone != TRUE) || (RecvDone != TRUE));

	auto rxMsg = receiveMsg();

	return true;
}

int PsCanController::setupInterruptSystem(XScuGic *IntcInstancePtr, XCanPs *CanInstancePtr, u16 CanIntrId)
{
	int Status;

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


	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
				     (Xil_ExceptionHandler)XScuGic_InterruptHandler,
				     IntcInstancePtr);


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
}
}
