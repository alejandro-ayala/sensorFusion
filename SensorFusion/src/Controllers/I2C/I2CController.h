#pragma once

#include "xiicps.h"

#include <stdint.h>

namespace Controllers
{
class I2CController
{
private:
	XIicPs_Config *Config;
public:
	I2CController();
	virtual ~I2CController() = default;

	void initialize();
	uint8_t receive(uint8_t reg_addr, uint8_t *pdata);
	uint8_t sendData(uint8_t reg_addr, uint8_t data);
};

}
