#include <Devices/LIDAR/GarminV3LiteCtrl.h>
#include "Devices/LIDAR/GarminV3LiteRegisterMap.h"
#include "services/Logger/LoggerMacros.h"
#include "services/Exception/SystemExceptions.h"
#include "sleep.h"
#include <iostream>

namespace hardware_abstraction
{
namespace Devices
{
GarminV3LiteCtrl::GarminV3LiteCtrl(std::unique_ptr<Controllers::I2CController>&& i2cCtrl , const LidarConfiguration& cfg) : m_i2cControl(std::move(i2cCtrl)), m_mode(cfg.sensorMode), m_addr(cfg.sensorAddr), m_initialized(false)
{


}

void GarminV3LiteCtrl::initialization()
{
	m_i2cControl->initialize();

	if(!m_initialized)
	{
		bool initialized = selfTest();
		if(!initialized)
		{
			THROW_CONTROLLERS_EXCEPTION(services::DevicesErrorId::LidarInitializationError, "Exception during LIDAR selftest");
		}
		configuration(m_mode);
	}
	else
	{
		LOG_WARNING("GarminV3LiteCtrl already initialized");
	}
}

void GarminV3LiteCtrl::configuration(GarminV3LiteMode mode)
{

	uint8_t txBuffer[2];
	txBuffer[0] = GarminV3LiteRegister::ACQ_COMMAND; // Register Addr;
	txBuffer[1] = 0x04; // Register Value;
	m_i2cControl->sendData(m_addr, txBuffer, sizeof(txBuffer) / sizeof(txBuffer[0]));

	switch(mode)
	{
	case GarminV3LiteMode::Balance:

		txBuffer[0] = GarminV3LiteRegister::SIG_COUNT_VAL; // Register Addr;
		txBuffer[1] = 0x80; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, sizeof(txBuffer) / sizeof(txBuffer[0]));

		txBuffer[0] = GarminV3LiteRegister::ACQ_CONFIG_REG; // Register Addr;
		txBuffer[1] = 0x04; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, sizeof(txBuffer) / sizeof(txBuffer[0]));

		txBuffer[0] = GarminV3LiteRegister::THRESHOLD_BYPASA; // Register Addr;
		txBuffer[1] = 0x00; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, sizeof(txBuffer) / sizeof(txBuffer[0]));
		break;

	case GarminV3LiteMode::ShortRange:

		txBuffer[0] = GarminV3LiteRegister::SIG_COUNT_VAL; // Register Addr;
		txBuffer[1] = 0x1d; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, sizeof(txBuffer) / sizeof(txBuffer[0]));

		txBuffer[0] = GarminV3LiteRegister::ACQ_CONFIG_REG; // Register Addr;
		txBuffer[1] = 0x08; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, sizeof(txBuffer) / sizeof(txBuffer[0]));

		txBuffer[0] = GarminV3LiteRegister::THRESHOLD_BYPASA; // Register Addr;
		txBuffer[1] = 0x00; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, sizeof(txBuffer) / sizeof(txBuffer[0]));

		break;

	case GarminV3LiteMode::DefaultRange:

		txBuffer[0] = GarminV3LiteRegister::SIG_COUNT_VAL; // Register Addr;
		txBuffer[1] = 0x80; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, sizeof(txBuffer) / sizeof(txBuffer[0]));

		txBuffer[0] = GarminV3LiteRegister::ACQ_CONFIG_REG; // Register Addr;
		txBuffer[1] = 0x08; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, sizeof(txBuffer) / sizeof(txBuffer[0]));

		txBuffer[0] = GarminV3LiteRegister::THRESHOLD_BYPASA; // Register Addr;
		txBuffer[1] = 0x00; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, sizeof(txBuffer) / sizeof(txBuffer[0]));
		break;


	case GarminV3LiteMode::MaximumRange:

		txBuffer[0] = GarminV3LiteRegister::SIG_COUNT_VAL; // Register Addr;
		txBuffer[1] = 0xff; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, sizeof(txBuffer) / sizeof(txBuffer[0]));

		txBuffer[0] = GarminV3LiteRegister::ACQ_CONFIG_REG; // Register Addr;
		txBuffer[1] = 0x08; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, sizeof(txBuffer) / sizeof(txBuffer[0]));

		txBuffer[0] = GarminV3LiteRegister::THRESHOLD_BYPASA; // Register Addr;
		txBuffer[1] = 0x00; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, sizeof(txBuffer) / sizeof(txBuffer[0]));
		break;

	case GarminV3LiteMode::HighSensitivityDetection:

		txBuffer[0] = GarminV3LiteRegister::SIG_COUNT_VAL; // Register Addr;
		txBuffer[1] = 0x80; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, sizeof(txBuffer) / sizeof(txBuffer[0]));

		txBuffer[0] = GarminV3LiteRegister::ACQ_CONFIG_REG; // Register Addr;
		txBuffer[1] = 0x08; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, sizeof(txBuffer) / sizeof(txBuffer[0]));

		txBuffer[0] = GarminV3LiteRegister::THRESHOLD_BYPASA; // Register Addr;
		txBuffer[1] = 0x80; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, sizeof(txBuffer) / sizeof(txBuffer[0]));
		break;

	case GarminV3LiteMode::LowSensitivityDetection:
		txBuffer[0] = GarminV3LiteRegister::SIG_COUNT_VAL; // Register Addr;
		txBuffer[1] = 0x80; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, sizeof(txBuffer) / sizeof(txBuffer[0]));

		txBuffer[0] = GarminV3LiteRegister::ACQ_CONFIG_REG; // Register Addr;
		txBuffer[1] = 0x08; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, sizeof(txBuffer) / sizeof(txBuffer[0]));

		txBuffer[0] = GarminV3LiteRegister::THRESHOLD_BYPASA; // Register Addr;
		txBuffer[1] = 0xb0; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, sizeof(txBuffer) / sizeof(txBuffer[0]));
		break;
	}
}

uint16_t GarminV3LiteCtrl::readDistance()
{

	uint8_t rxBuffer[16];
	uint8_t txBuffer[2];
	memset(rxBuffer, 0, sizeof(rxBuffer));

	txBuffer[0] = GarminV3LiteRegister::ACQ_COMMAND; // Register Addr;
	txBuffer[1] = 0x04; // Register Value;
	LOG_TRACE("readDistance SENDING GarminV3LiteRegister::ACQ_COMMAND");
	m_i2cControl->sendData(m_addr, txBuffer, sizeof(txBuffer) / sizeof(txBuffer[0]));
	LOG_TRACE("readDistance reading GarminV3LiteRegister::STATUS");
	uint8_t registerAddr = GarminV3LiteRegister::STATUS;
	m_i2cControl->readData(m_addr, registerAddr, rxBuffer, 1);
	while((rxBuffer[0] & 0x01) != 0)
	{
		m_i2cControl->readData(m_addr, registerAddr, rxBuffer, 1);

		//TODO review it
		for (int Delay = 0; Delay < 0x3FF; Delay++);
	}
	LOG_TRACE("readDistance READED GarminV3LiteRegister::STATUS");
	registerAddr = GarminV3LiteRegister::DISTANCE_CM;
	memset(rxBuffer, 0, sizeof(rxBuffer));
	m_i2cControl->readData(m_addr, registerAddr, rxBuffer, 2);
	LOG_TRACE("readDistance READED GarminV3LiteRegister::DISTANCE_CM");
	uint16_t distance = ((rxBuffer[0] << 8) | rxBuffer[1]);
	return distance;
}

bool GarminV3LiteCtrl::applyBiasCorrection()
{

}

bool GarminV3LiteCtrl::selfTest()
{
	auto selfTest = true;
	if(selfTest)
	{
		bool selfTestResult = false;

		uint8_t registerAddr= GarminV3LiteRegister::UNIT_ID_HIGH;
		uint8_t buffer[2] = {0};
		m_i2cControl->readData(m_addr, registerAddr, buffer, 2);
		uint16_t lidarSerialNumber = (buffer[0] << 8) | buffer[1];

		if(lidarSerialNumber == expectedLidarSN)
		{
			selfTestResult = true;
		}

		return selfTestResult;
	}
	else
	{
		if(enableTestMode())
		{
			runTestMode();
		}
	}

}

bool GarminV3LiteCtrl::enableTestMode()
{
		bool testModeEnabled;
		uint8_t registerAddr= GarminV3LiteRegister::COMMAND;
		uint8_t buffer[15];
		m_i2cControl->readData(m_addr, registerAddr, buffer, 1); // Read State cmd
		uint16_t stateValue = buffer[0];
		if(stateValue != 0)
		{
			//TODO handle error
			LOG_ERROR("Lidar is on error: " , stateValue);
			testModeEnabled = false;
		}
		else
		{
			buffer[0] = GarminV3LiteRegister::ACQ_SETTINGS;
			buffer[1] = 0xC0;
			m_i2cControl->sendData(m_addr, buffer, 2); // Enable correlation memory bank

			registerAddr = GarminV3LiteRegister::ACQ_SETTINGS;
			buffer[0] = 0x00;
			m_i2cControl->readData(m_addr, registerAddr, buffer, 1); // Check correlation memory bank is enabled
			if(buffer[0] != 0xC0)
			{
				LOG_FATAL("Lidar correlation memory bank is NOT enabled");
				testModeEnabled = false;
			}
			else
			{
				buffer[0] = GarminV3LiteRegister::COMMAND;
				buffer[1] = 0x07;
				m_i2cControl->sendData(m_addr, buffer, 2); // Enable test mode
				testModeEnabled = true;
			}
		}
		return testModeEnabled;
}

bool GarminV3LiteCtrl::runTestMode()
{
	uint8_t buffer[2]= {0};
	uint16_t testData = 0;
	int16_t testDataPrev = -1;
	uint8_t registerAddr = GarminV3LiteRegister::TEST_MODE_DATA;
	bool testResult = true;

	for(int testCycles = 0; (testCycles < 10 && testResult) ; testCycles++)
	{

		buffer[0] = 0x00;
		buffer[1] = 0x00;
		m_i2cControl->readData(m_addr, registerAddr, buffer, 2); // Read correclation data
		testData = (buffer[1] << 8) | buffer[0];
		if(testData == (testDataPrev + 1))
		{
			LOG_TRACE("runTestMode -- testData: " , testData);
			testDataPrev = testData;
		}
		else
		{
			testResult = false;
			LOG_FATAL("runTestMode -- testData ERROR. Data:  " , testData , "testDataPrev: ", testDataPrev);
		}

	}
	return testResult;
}
}
}
