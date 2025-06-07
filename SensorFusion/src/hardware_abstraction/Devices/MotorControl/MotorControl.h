#pragma once

#include <hardware_abstraction/Controllers/IController.h>
#include <hardware_abstraction/Devices/MotorControl/IHbridge.h>
#include "MotorChannelsEnum.h"

namespace hardware_abstraction
{
namespace Devices
{
namespace MotorControl
{
class MotorControl : public Controllers::IController
{
private:
	IHbridge* hbridge;
public:
	MotorControl(IHbridge* hbridge);
	virtual ~MotorControl();

	void initialize() override;
	void turnOn(MOTOR_CHANNEL ch);
	void turnOff(MOTOR_CHANNEL ch);
	void forward(MOTOR_CHANNEL ch);
	void backward(MOTOR_CHANNEL ch);
	//void setVelocity(MOTOR_CHANNEL ch, uint32_t speed);
	bool selfTest() override;
};
}
}
}
