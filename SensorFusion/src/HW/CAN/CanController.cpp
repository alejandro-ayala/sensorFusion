#include "CanController.h"
#include "CanCommandsRegisters.h"

using namespace Hardware;
namespace Communication
{

CanController::CanController() : ICommunication("CanController")
{
	xspiConfig.spiConfiguration = {0,0,1,0,1,8,0,0,0,0,0};
	xspiConfig.gpioAddress      = XPAR_PMODCAN_0_AXI_LITE_GPIO_BASEADDR;
	xspiConfig.spiBaseAddress   = XPAR_PMODCAN_0_AXI_LITE_SPI_BASEADDR;
	spiControl = new SPIController(xspiConfig);
}

CanController::~CanController()
{
	delete spiControl;
}

void CanController::initialize()
{

   // 0b1111 for input 0b0000 for output, 0b0001 for pin1 in pin 2 out etc.
   Xil_Out32(xspiConfig.gpioAddress + 4, 0b1111);
   //CAN_SPIInit(&InstancePtr->CANSpi);
   spiControl->initialize();


   configureTransceiver(CAN_ModeNormalOperation);
}

void CanController::configureTransceiver(uint8_t mode)
{
	uint8_t CNF[3] = {0x86, 0xFB, 0x41};

   // Set CAN control mode to configuration
   modifyRegister(CAN_CANCTRL_REG_ADDR, CAN_CAN_CANCTRL_MODE_MASK, CAN_ModeConfiguration);

   // Set config rate and clock for CAN
   writeRegister(CAN_CNF3_REG_ADDR, CNF, 3);

   clearRegister(0x00, 12); // Initiate CAN buffer filters and
   clearRegister(0x10, 12); // registers
   clearRegister(0x20, 8);
   clearRegister(0x30, 14);
   clearRegister(0x40, 14);
   clearRegister(0x50, 14);

   // Set the CAN mode for any message type
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

int CanController::transmitMsg(uint8_t idMsg, uint8_t *txMsg, uint8_t msgLength)
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

	//xil_printf("Waiting to send\r\n");
	do
	{
	  status = spiControl->readRegister(CAN_READSTATUS_CMD);
	} while ((status & CAN_STATUS_TX0REQ_MASK) != 0); // Wait for buffer 0 to be clear


	//xil_printf("sending ");


	modifyRegister(CAN_CANINTF_REG_ADDR, CAN_CANINTF_TX0IF_MASK, 0);

	//xil_printf("requesting to transmit message through transmit buffer 0 \\r\n");


	transmit(message);

	modifyRegister(CAN_CANINTF_REG_ADDR, CAN_CANINTF_TX0IF_MASK, 0);

	do
	{
	 status = spiControl->readRegister(CAN_READSTATUS_CMD);//CAN_ReadStatus(&myDevice);
	 //xil_printf("Waiting to complete transmission\r\n");
	} while ((status & CAN_STATUS_TX0IF_MASK) != 0); // Wait for message to transmit successfully
	return 1;
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
	CAN_TxBuffer target = CAN_Tx0;
	switch (target)
	{
	case CAN_Tx0:
	  rts_mask = CAN_RTS_TXB0_MASK;
	  load_start_addr = CAN_LOADBUF_TXB0SIDH;
	  break;
	case CAN_Tx1:
	  rts_mask = CAN_RTS_TXB1_MASK;
	  load_start_addr = CAN_LOADBUF_TXB1SIDH;
	  break;
	case CAN_Tx2:
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

//	//xil_printf("CAN_SendMessage msg.dlc: %02x\r\n", msg.dlc);
//	for (uint8_t i = 0; i < 5 + msg.dlc; i++)
//	  //xil_printf("CAN_SendMessage: %02x\r\n", data[i]);

	//CAN_LoadTxBuffer(InstancePtr, load_start_addr, data, message.dlc + 5);
	loadTxBuffer(load_start_addr, data, msg.dlc + 5);
	//CAN_RequestToSend(InstancePtr, rts_mask);
	requestToSend(rts_mask);
}

void CanController::receive(CanFrame *receiveMsg, CAN_RxBuffer target)
{
	uint8_t data[13];
	uint8_t read_start_addr;

	switch (target)
	{
	case CAN_Rx0:
		read_start_addr = CAN_READBUF_RXB0SIDH;
		break;
	case CAN_Rx1:
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

	// Read only relevant data bytes
	//spiControl->readData(CAN_READBUF_CMD | read_start_addr, data, receiveMsg->dlc);

	for (uint8_t i = 0; i < receiveMsg->dlc; i++)
		receiveMsg->data[i] = data[i + 5];
}

int CanController::receiveMsg(uint8_t *rxBuffer)
{
	CanFrame rxMsg;
	CAN_RxBuffer target;
	uint8_t status;
	uint8_t rx_int_mask;

	do {
	 status = spiControl->readRegister(CAN_READSTATUS_CMD);
	 ////xil_printf("\r\nWaiting to receive\r\n");
	} while ((status & CAN_STATUS_RX0IF_MASK) != 0 && (status & CAN_STATUS_RX1IF_MASK) != 0);

	switch (status & 0x03) {
	case 0b01:
	case 0b11:
	 //xil_printf("fetching message from receive buffer 0\r\n");
	 target = CAN_Rx0;
	 rx_int_mask = CAN_CANINTF_RX0IF_MASK;
	 break;
	case 0b10:
	 //xil_printf("fetching message from receive buffer 1\r\n");
	 target = CAN_Rx1;
	 rx_int_mask = CAN_CANINTF_RX1IF_MASK;
	 break;
	default:
	 ////xil_printf("Error, message not received\r\n");
	 return 0;
	}

	receive(&rxMsg, target);
	modifyRegister(CAN_CANINTF_REG_ADDR, rx_int_mask, 0);
	//xil_printf("received ");
	rxBuffer[0] = rxMsg.id;
	for(uint32_t i=0; i<rxMsg.dlc;i++)
	{
		rxBuffer[i+1] = rxMsg.data[i];
	}
	return rxMsg.dlc;
}

bool CanController::selfTest()
{
	return true;
}
}
