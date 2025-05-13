#pragma once

#include <hardware_abstraction/Controllers/I2C/I2CConfig.h>
#include <vector>
#define I2C_POLLING
namespace hardware_abstraction
{
namespace Controllers
{
class I2CController
{
private:
	I2CConfig m_config;
	uint8_t m_i2cDeviceId = 0;
	uint8_t m_intCtrlDeviceId = XPAR_SCUGIC_SINGLE_DEVICE_ID;
	uint8_t m_i2cIntVectorId = XPS_I2C0_INT_ID;
	static volatile inline uint32_t sendComplete;
	static volatile inline uint32_t recvComplete;
	static volatile inline uint32_t totalErrorCount;
	static volatile inline uint32_t recvError;
	static volatile inline uint32_t totalRecvErrorCount;
public:
	I2CController(const I2CConfig& config = I2CConfig());
	virtual ~I2CController() = default;

	void initialize();
	uint8_t readData(uint8_t slaveAddr, uint8_t registerAddr, uint8_t *buffer, uint8_t bufferSize);
	uint8_t sendData(uint8_t regAddr, uint8_t *buffer, uint32_t bufferSize);

#ifndef I2C_POLLING
	int SetupInterruptSystem(XIicPs *IicPsPtr);
	static void irqHandler(void *callBackRef, u32 event);
#endif
	bool selfTest();
};
}
}
