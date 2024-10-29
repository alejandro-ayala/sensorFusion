#pragma once

#include <stdint.h>
#include "xiicps.h"
#include "xscugic.h"

namespace hardware_abstraction
{
namespace Controllers
{
struct I2CConfig
{
	uint8_t deviceId = XPAR_XIICPS_0_DEVICE_ID;
	uint32_t clockRate = 100000;
	XIicPs_Config* i2cPsConfig;
	XIicPs i2cPsInstance;			/* Instance of the IIC Device */
	XScuGic InterruptController;	/* Instance of the Interrupt Controller */
	uint8_t intDeviceId = XPAR_SCUGIC_SINGLE_DEVICE_ID;
};
}
}
