#pragma once
#include <business_logic/Communication/ICommunication.h>
#include <hardware_abstraction/Controllers/CAN/CanFrame.h>
#include <hardware_abstraction/Controllers/IController.h>
#include <hardware_abstraction/Controllers/SPI/SPIController.h>
#include "sleep.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "business_logic/Osal/MutexHandler.h"
#include <memory>


namespace hardware_abstraction
{
namespace Controllers
{
class CanController
{
private:
	std::shared_ptr<Controllers::SPIController> spiControl;
	SPIConfig xspiConfig;
	int deviceId;
	bool m_initialized;
	std::shared_ptr<business_logic::Osal::MutexHandler> canMutex;

	void modifyRegister(uint8_t reg, uint8_t mask, uint8_t value);
	void writeRegister(uint8_t reg, uint8_t *data, uint32_t nData);
	void clearRegister(uint8_t reg, uint32_t nData);
	void configureTransceiver(uint8_t mode);
	void requestToSend(uint8_t mask);
	void loadTxBuffer(uint8_t start_addr, uint8_t *data, uint32_t nData);
	void transmit(CanFrame msg);
	void receive(CanFrame *receiveMsg, CanRxBuffer target);
public:
	CanController();
	virtual ~CanController();

	virtual void initialize();
	bool transmitMsg(uint8_t idMsg, uint8_t *TxMsg, uint8_t msgLength);
	CanFrame receiveMsg();
	bool selfTest();
};
}
}
