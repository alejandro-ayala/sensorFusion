#include "GarminV3LiteCtrl.h"

namespace Devices
{
GarminV3LiteCtrl::GarminV3LiteCtrl(const std::shared_ptr<Controllers::I2CController>& i2cCtrl , const LidarConfiguration& cfg) : m_i2cControl(i2cCtrl), m_mode(cfg.sensorMode), m_addr(cfg.sensorAddr)
{


}

void GarminV3LiteCtrl::initialization()
{
	bool initialized = selfTest();

	configuration(m_mode);
}

void GarminV3LiteCtrl::configuration(GarminV3LiteMode mode)
{

	uint8_t txBuffer[16];
	txBuffer[0] = 0x00; // Register Addr;
	txBuffer[1] = 0x04; // Register Value;
	m_i2cControl->sendData(m_addr, txBuffer, 2);

	switch(mode)
	{
	case GarminV3LiteMode::Balance:

		txBuffer[0] = 0x02; // Register Addr;
		txBuffer[1] = 0x80; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, 2);

		txBuffer[0] = 0x04; // Register Addr;
		txBuffer[1] = 0x04; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, 2);

		txBuffer[0] = 0x1c; // Register Addr;
		txBuffer[1] = 0x00; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, 2);
		break;

	case GarminV3LiteMode::ShortRange:

		txBuffer[0] = 0x02; // Register Addr;
		txBuffer[1] = 0x1d; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, 2);

		txBuffer[0] = 0x04; // Register Addr;
		txBuffer[1] = 0x08; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, 2);

		txBuffer[0] = 0x1c; // Register Addr;
		txBuffer[1] = 0x00; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, 2);

		break;

	case GarminV3LiteMode::DefaultRange:

		txBuffer[0] = 0x02; // Register Addr;
		txBuffer[1] = 0x80; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, 2);

		txBuffer[0] = 0x04; // Register Addr;
		txBuffer[1] = 0x08; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, 2);

		txBuffer[0] = 0x1c; // Register Addr;
		txBuffer[1] = 0x00; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, 2);
		break;


	case GarminV3LiteMode::MaximumRange:

		txBuffer[0] = 0x02; // Register Addr;
		txBuffer[1] = 0xff; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, 2);

		txBuffer[0] = 0x04; // Register Addr;
		txBuffer[1] = 0x08; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, 2);

		txBuffer[0] = 0x1c; // Register Addr;
		txBuffer[1] = 0x00; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, 2);
		break;

	case GarminV3LiteMode::HighSensitivityDetection:

		txBuffer[0] = 0x02; // Register Addr;
		txBuffer[1] = 0x80; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, 2);

		txBuffer[0] = 0x04; // Register Addr;
		txBuffer[1] = 0x08; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, 2);

		txBuffer[0] = 0x1c; // Register Addr;
		txBuffer[1] = 0x80; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, 2);
		break;

	case GarminV3LiteMode::LowSensitivityDetection:
		txBuffer[0] = 0x02; // Register Addr;
		txBuffer[1] = 0x80; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, 2);

		txBuffer[0] = 0x04; // Register Addr;
		txBuffer[1] = 0x08; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, 2);

		txBuffer[0] = 0x1c; // Register Addr;
		txBuffer[1] = 0xb0; // Register Value;
		m_i2cControl->sendData(m_addr, txBuffer, 2);
		break;
	}
}

uint16_t GarminV3LiteCtrl::readDistance()
{
	uint8_t txBuffer[16];
	txBuffer[0] = 0x00; // Register Addr;
	txBuffer[1] = 0x04; // Register Value;
	m_i2cControl->sendData(m_addr, txBuffer, 2);

	uint8_t registerAddr = 0x01;
	uint8_t rxBuffer[16];

	while((rxBuffer[0] && 0x01) != 1)
	{
		m_i2cControl->readData(m_addr, registerAddr, rxBuffer, 1);

		//TODO review it
		for (int Delay = 0; Delay < 10000000; Delay++);
	}

	registerAddr = 0x8F;
	m_i2cControl->readData(m_addr, registerAddr, rxBuffer, 2);

	uint16_t distance = ((rxBuffer[0] << 8) | rxBuffer[1]);
	return distance;
}

bool GarminV3LiteCtrl::selfTest()
{
//	bool selfTestResult = false;
//	const uint64_t expectedLidarSN = 0x00;
//	uint8_t registerAddr= 0x16;
//	uint8_t buffer[15];
//	m_i2cControl->readData(m_addr, registerAddr, buffer, 1);
//	uint16_t lidarSerialNumber = buffer[0];
//	buffer[0] = 0;
//	registerAddr= 0x17;
//	m_i2cControl->readData(m_addr, registerAddr, buffer, 1);
//	lidarSerialNumber |= (buffer[0]<<8);
//
//	if(lidarSerialNumber == expectedLidarSN)
//	{
//		selfTestResult = true;
//	}
//	return selfTestResult;

	bool selfTestResult = false;
	uint8_t registerAddr= 0x40;
	uint8_t buffer[15];
	m_i2cControl->readData(m_addr, registerAddr, buffer, 1); // Read State cmd
	uint16_t stateValue = buffer[0];

	buffer[0] = 0xC0;
	registerAddr= 0x5d;
	m_i2cControl->sendData(m_addr, buffer, 2); // Enable correlation memory bank

	buffer[0] = 0x00;
	m_i2cControl->readData(m_addr, registerAddr, buffer, 1); // Check correlation memory bank is enabled

	buffer[0] = 0x07;
	registerAddr= 0x40;
	m_i2cControl->sendData(m_addr, buffer, 2); // Enable test mode

	while(1)
	{
		registerAddr = 0x52;
		buffer[0] = 0x00;
		buffer[1] = 0x00;
		m_i2cControl->readData(m_addr, registerAddr, buffer, 2); // Read correclation data
	}
	return selfTestResult;
}

}
