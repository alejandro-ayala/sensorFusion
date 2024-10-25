#pragma once

#include <stdint.h>

enum class GarminV3LiteMode
{
	Balance,
	ShortRange,
	DefaultRange,
	MaximumRange,
	HighSensitivityDetection,
	LowSensitivityDetection
};

struct LidarConfiguration
{
	GarminV3LiteMode sensorMode;
	uint8_t sensorAddr;
};
