#pragma once
#include "sleep.h"
#include "xil_cache.h"
#include "xparameters.h"

#include "HW/IController.h"
#include "ICommunication.h"
#include "HW/SPIController.h"
#include "CanFrame.h"

namespace Communication
{
class CanController : public ICommunication
{
private:
	Hardware::SPIController* spiControl;
	SPIConfig xspiConfig;
	int deviceId;

	void modifyRegister(uint8_t reg, uint8_t mask, uint8_t value);
	void writeRegister(uint8_t reg, uint8_t *data, uint32_t nData);
	void clearRegister(uint8_t reg, uint32_t nData);
	void configureTransceiver(uint8_t mode);
	void requestToSend(uint8_t mask);
	void loadTxBuffer(uint8_t start_addr, uint8_t *data, uint32_t nData);
	void transmit(CanFrame msg);
	void receive(CanFrame *receiveMsg, CAN_RxBuffer target);
public:
	CanController();
	virtual ~CanController();

	virtual void initialize() override;
	int transmitMsg(uint8_t idMsg, uint8_t *TxMsg, uint8_t msgLength) override;
	int receiveMsg(uint8_t *rxBuffer) override;
	bool selfTest() override;
};
}
