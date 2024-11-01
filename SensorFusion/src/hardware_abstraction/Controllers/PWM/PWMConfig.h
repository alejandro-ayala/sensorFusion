#pragma once
#include <stdint.h>

namespace hardware_abstraction
{
namespace Controllers
{
struct PWMConfig
{
	uint32_t pwmPeriodScaler = 1000000;
	uint8_t  pwmIndex = 1;
	uint8_t  pwmFreq = 50;
};
}
}
