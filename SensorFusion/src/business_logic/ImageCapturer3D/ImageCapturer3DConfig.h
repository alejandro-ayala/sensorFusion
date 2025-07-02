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

	uint8_t fov = 40;
	uint8_t minVerticalAngle = 90;
	uint8_t minHorizontalAngle = 60;
	uint8_t maxVerticalAngle = minVerticalAngle + fov;
	uint8_t maxHorizontalAngle = minHorizontalAngle + fov;
	uint8_t horizontalAngleResolution = 1;
	uint8_t verticalAngleResolution = 1;
	uint8_t offsetVerticalAngle = 110;
	uint8_t offsetHorizontalAngle = 80;
	uint8_t settlingTime = 5;

	bool applyBiasCorrection = false;
	uint8_t samplesPerPosition = IMAGE3D_SAMPLES_PER_POSITION;
};
}
