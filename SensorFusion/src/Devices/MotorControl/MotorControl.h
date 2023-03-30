#pragma once

#include <MotorControl/IHbridge.h>
#include "TestInclude.h"
#include "MotorChannelsEnum.h"
#include "IController.h"

namespace Components
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

	TVIRTUAL void initialize() override;
	void turnOn(MOTOR_CHANNEL ch);
	void turnOff(MOTOR_CHANNEL ch);
	void forward(MOTOR_CHANNEL ch);
	void backward(MOTOR_CHANNEL ch);
	//void setVelocity(MOTOR_CHANNEL ch, uint32_t speed);
	bool selfTest() override;
};
}
}
