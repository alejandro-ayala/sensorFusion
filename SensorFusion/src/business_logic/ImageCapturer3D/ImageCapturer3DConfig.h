#pragma once

#include "hardware_abstraction/Devices/ServoMotor/ServoMotorControl.h"
#include "hardware_abstraction/Devices/LIDAR/GarminV3LiteCtrl.h"
#include "Devices/ServoMotor/ServoMotorControl.h"
#include <stdint.h>

static const uint16_t IMAGE3D_SIZE = 2048;
static const uint8_t IMAGE3D_SAMPLES_PER_POSITION = 1;

namespace business_logic
{
struct ImageCapturer3DConfig
{
	std::shared_ptr<hardware_abstraction::Devices::ServoMotorControl> verServoCtrl;
	std::shared_ptr<hardware_abstraction::Devices::ServoMotorControl> horServoCtrl;
	std::shared_ptr<hardware_abstraction::Devices::GarminV3LiteCtrl>  lidarCtrl;
	uint8_t initVerticalAngle = 60;
	uint8_t initHorizontalAngle = 20;
	uint8_t maxVerticalAngle = 150;
	uint8_t maxHorizontalAngle = 180;
	uint8_t horizontalAngleResolution = 2;
	uint8_t verticalAngleResolution = 5;
	bool applyBiasCorrection = false;
	uint8_t samplesPerPosition = IMAGE3D_SAMPLES_PER_POSITION;
};
}
