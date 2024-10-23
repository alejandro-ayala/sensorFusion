#pragma once

#include "I2CConfig.h"

#include <vector>


namespace Controllers
{
class I2CController
{
private:
	I2CConfig m_config;
	uint8_t m_i2cDeviceId = 0;
	uint8_t m_intCtrlDeviceId = 0;
	uint8_t m_i2cIntVectorId = XPS_I2C0_INT_ID;

public:
	I2CController(const I2CConfig& config);
	virtual ~I2CController() = default;

	void initialize();
	int SetupInterruptSystem(XIicPs *IicPsPtr);
	uint8_t readData(uint8_t slaveAddr, uint8_t registerAddr, uint8_t *buffer, uint8_t bufferSize);
	uint8_t sendData(uint8_t regAddr, const std::vector<uint8_t>& buffer);
	bool selfTest();
};

}
