#include <hardware_abstraction/Controllers/CAN/CanCommandsRegisters.h>
#include <hardware_abstraction/Controllers/CAN/CanController.h>
#include "services/Logger/LoggerMacros.h"

namespace hardware_abstraction
{
namespace Controllers
{

constexpr uint8_t READ_STATUS_REG_RETRIES = 10;

CanController::CanController() : m_initialized(false)
{
	canMutex = std::make_shared<business_logic::Osal::MutexHandler>();
	xspiConfig.spiConfiguration = {0,0,1,0,1,8,0,0,0,0,0};
	xspiConfig.gpioAddress      = XPAR_PMODCAN_0_AXI_LITE_GPIO_BASEADDR;
	xspiConfig.spiBaseAddress   = XPAR_PMODCAN_0_AXI_LITE_SPI_BASEADDR;
	spiControl = std::make_shared<SPIController>(xspiConfig);
}

CanController::~CanController()
{
}

void CanController::initialize()
{
	if(!m_initialized)
	{
	   // 0b1111 for input 0b0000 for output, 0b0001 for pin1 in pin 2 out etc.
	   Xil_Out32(xspiConfig.gpioAddress + 4, 0b1111);
	   //CAN_SPIInit(&InstancePtr->CANSpi);
	   spiControl->initialize();


	   configureTransceiver(static_cast<uint8_t>(CanMode::NormalOperation));
	   m_initialized = true;
	}
	else
	{
		LOG_WARNING("CanController already initialized");
	}
}

void CanController::configureTransceiver(uint8_t mode)
{
	uint8_t CNF[3] = {0x86, 0xFB, 0x41};

   // Set CAN control mode to configuration
   modifyRegister(CAN_CANCTRL_REG_ADDR, CAN_CAN_CANCTRL_MODE_MASK, static_cast<uint8_t>(CanMode::Configuration));

   // Set config rate and clock for CAN
   writeRegister(CAN_CNF3_REG_ADDR, CNF, 3);

   clearRegister(0x00, 12); // Initiate CAN buffer filters and
   clearRegister(0x10, 12); // registers
   clearRegister(0x20, 8);
   clearRegister(0x30, 14);
   clearRegister(0x40, 14);
   clearRegister(0x50, 14);

   modifyRegister(CAN_RXB0CTRL_REG_ADDR, 0x64, 0x60);

   // Set CAN control mode to selected mode (exit configuration)
   modifyRegister(CAN_CANCTRL_REG_ADDR, CAN_CAN_CANCTRL_MODE_MASK, mode << CAN_CANCTRL_MODE_BIT);
}

void CanController::modifyRegister(uint8_t reg, uint8_t mask, uint8_t value)
{
	uint8_t buf[4] = {CAN_MODIFY_REG_CMD, reg, mask, value};
	spiControl->writeData(buf, 4, NULL, 0);
}


void CanController::writeRegister(uint8_t reg, uint8_t *data, uint32_t nData)
{
	uint8_t buf[nData + 2];
	uint32_t i;
	buf[0] = CAN_WRITE_REG_CMD;
	buf[1] = reg;
	for (i = 0; i < nData; i++)
	  buf[i + 2] = data[i];

	spiControl->writeData(buf, nData + 2, NULL, 0);
}

void CanController::clearRegister(uint8_t reg, uint32_t nData)
{
	uint8_t buf[nData + 2];
	buf[0] = CAN_WRITE_REG_CMD;
	buf[1] = reg;
	spiControl->writeData(buf, nData + 2, NULL, 0);
}

bool CanController::transmitMsg(uint8_t idMsg, uint8_t *txMsg, uint8_t msgLength)
{
	CanFrame message;
	uint8_t status;
	message.id  = idMsg;
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
	while (status = spiControl->readRegister(CAN_READSTATUS_CMD), (status & CAN_STATUS_TX0REQ_MASK) != 0)
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

	modifyRegister(CAN_CANINTF_REG_ADDR, CAN_CANINTF_TX0IF_MASK, 0);

	transmit(message);

	modifyRegister(CAN_CANINTF_REG_ADDR, CAN_CANINTF_TX0IF_MASK, 0);

	waitStatusRegisterRetries = 0;
	while (status = spiControl->readRegister(CAN_READSTATUS_CMD), (status & CAN_STATUS_TX0IF_MASK) != 0)
	{
		waitStatusRegisterRetries++;
		if(waitStatusRegisterRetries > READ_STATUS_REG_RETRIES)
		{
			LOG_WARNING("Spi status register is not ready after transmission");
			canMutex->unlock();
			return false;
		}
		usleep(100);
	}

	canMutex->unlock();
	return true;
}

void CanController::loadTxBuffer(uint8_t start_addr, uint8_t *data, uint32_t nData)
{
	uint8_t buf[nData + 1];
	buf[0] = CAN_LOADBUF_CMD | start_addr;
	for (uint8_t i = 0; i < nData; i++)
		buf[i + 1] = data[i];

	spiControl->writeData(buf, nData + 1, NULL, 0);
}

void CanController::requestToSend(uint8_t mask)
{
	uint8_t buf[1] = {CAN_RTS_CMD | mask};
	spiControl->writeData(buf, 1, NULL, 0);
}

void CanController::transmit(CanFrame msg)
{
	uint8_t data[13];
	uint8_t rts_mask;
	uint8_t load_start_addr;
	CanTxBuffer target = hardware_abstraction::Controllers::CanTxBuffer::CanTx0;
	switch (target)
	{
	case hardware_abstraction::Controllers::CanTxBuffer::CanTx0:
	  rts_mask = CAN_RTS_TXB0_MASK;
	  load_start_addr = CAN_LOADBUF_TXB0SIDH;
	  break;
	case hardware_abstraction::Controllers::CanTxBuffer::CanTx1:
	  rts_mask = CAN_RTS_TXB1_MASK;
	  load_start_addr = CAN_LOADBUF_TXB1SIDH;
	  break;
	case hardware_abstraction::Controllers::CanTxBuffer::CanTx2:
	  rts_mask = CAN_RTS_TXB2_MASK;
	  load_start_addr = CAN_LOADBUF_TXB2SIDH;
	  break;
	default:
	  return XST_FAILURE;
	}

	data[0] = (msg.id >> 3) & 0xFF; // TXB0 SIDH

	data[1] = (msg.id << 5) & 0xE0; // TXB0 SIDL
	data[1] |= (msg.ide << 3) & 0x08;
	data[1] |= (msg.eid >> 16) & 0x03;

	data[2] = (msg.eid >> 8) & 0xFF;
	data[3] = (msg.eid) & 0xFF;

	data[4] = (msg.rtr << 6) & 0x40;
	data[4] |= (msg.dlc) & 0x0F;

	for (uint8_t i = 0; i < msg.dlc; i++)
	  data[i + 5] = msg.data[i];

	loadTxBuffer(load_start_addr, data, msg.dlc + 5);
	requestToSend(rts_mask);
}

void CanController::receive(CanFrame *receiveMsg, CanRxBuffer target)
{
	uint8_t data[13];
	uint8_t read_start_addr;

	switch (target)
	{
	case CanRxBuffer::CanRx0:
		//LOG_INFO("CAN_Rx0 received");
		read_start_addr = CAN_READBUF_RXB0SIDH;
		break;
	case CanRxBuffer::CanRx1:
		read_start_addr = CAN_READBUF_RXB1SIDH;
		break;
	default:
		return XST_FAILURE;
	}

	spiControl->readData(CAN_READBUF_CMD | read_start_addr, data, 13);

	receiveMsg->id = (u16) data[0] << 3;
	receiveMsg->id |= (data[1] & 0xE0) >> 5;

	receiveMsg->ide = (data[1] & 0x08) >> 3;

	receiveMsg->srr = (data[1] & 0x10) >> 4;

	receiveMsg->eid = (u32) (data[1] & 0x03) << 16;
	receiveMsg->eid |= (u32) (data[2] & 0xFF) << 8;
	receiveMsg->eid |= (u32) (data[3] & 0xFF);

	receiveMsg->rtr = (data[4] & 0x40) >> 6;

	receiveMsg->dlc = data[4] & 0x0F;

	for (uint8_t i = 0; i < receiveMsg->dlc; i++)
		receiveMsg->data[i] = data[i + 5];
}

CanFrame CanController::receiveMsg()
{

	CanRxBuffer target;
	uint8_t status;
	uint8_t rx_int_mask;
	CanFrame rxMsg;
	canMutex->lock();
	uint8_t waitStatusRegisterRetries = 0;
	while (status = spiControl->readRegister(CAN_READSTATUS_CMD), (status & CAN_STATUS_RX0IF_MASK) != 0 && (status & CAN_STATUS_RX1IF_MASK) != 0)
	{
		waitStatusRegisterRetries++;
		if(waitStatusRegisterRetries > READ_STATUS_REG_RETRIES)
		{
			LOG_WARNING("Spi status register is not ready to receive");
			canMutex->unlock();
			status = 0;
			break;
		}
		usleep(100);
	}

	switch (status & 0x03)
	{
	case 0b01:
	case 0b11:
		 target = CanRxBuffer::CanRx0;
		 rx_int_mask = CAN_CANINTF_RX0IF_MASK;
		 break;
	case 0b10:
		 target = CanRxBuffer::CanRx1;
		 rx_int_mask = CAN_CANINTF_RX1IF_MASK;
		 break;
	default:
		target = CanRxBuffer::None;
		rxMsg.dlc = 0;
		break;
	}

	if(target != CanRxBuffer::None)
	{
		receive(&rxMsg, target);
		modifyRegister(CAN_CANINTF_REG_ADDR, rx_int_mask, 0);
	}

	canMutex->unlock();
	return rxMsg;
}

bool CanController::selfTest()
{
	return true;
}
}
}
