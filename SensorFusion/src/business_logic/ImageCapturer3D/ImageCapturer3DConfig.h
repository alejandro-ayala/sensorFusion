#pragma once

#include "hardware_abstraction/Devices/ServoMotor/ServoMotorControl.h"
#include "hardware_abstraction/Devices/LIDAR/GarminV3LiteCtrl.h"
#include "Devices/ServoMotor/ServoMotorControl.h"
#include <stdint.h>

static const uint8_t IMAGE3D_SAMPLES_PER_POSITION = 1;

namespace business_logic
{
struct ImageCapturer3DConfig
{
	std::shared_ptr<hardware_abstraction::Devices::ServoMotorControl> verServoCtrl;
	std::shared_ptr<hardware_abstraction::Devices::ServoMotorControl> horServoCtrl;
	std::shared_ptr<hardware_abstraction::Devices::GarminV3LiteCtrl>  lidarCtrl;
	uint8_t minVerticalAngle = 120;
	uint8_t minHorizontalAngle = 114;
	uint8_t maxVerticalAngle = 160;
	uint8_t maxHorizontalAngle = 230;
	uint8_t horizontalAngleResolution = 2;
	uint8_t verticalAngleResolution = 2;
	uint8_t settlingTime = 20;
	bool applyBiasCorrection = false;
	uint8_t samplesPerPosition = IMAGE3D_SAMPLES_PER_POSITION;
};
}
