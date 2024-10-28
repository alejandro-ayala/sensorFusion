#pragma once

#include "xspi.h"
#include <stdint.h>
namespace hardware_abstraction
{
namespace Controllers
{
typedef struct
{
	XSpi_Config spiConfiguration;
	uint32_t    gpioAddress;
	uint32_t    spiBaseAddress;
} SPIConfig;
}
}
