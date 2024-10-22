#include "I2CController.h"


namespace Controllers
{

I2CController::I2CController()
{

}

void I2CController::initialize()
{

}

uint8_t I2CController::receive(uint8_t reg_addr, uint8_t *pdata)
{
	uint8_t opertionStatus = 0;

	return opertionStatus;
}

uint8_t I2CController::sendData(uint8_t reg_addr, uint8_t data)
{
	uint8_t opertionStatus = 0;

	return opertionStatus;
}
}
