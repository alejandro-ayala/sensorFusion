#pragma once

#include "IHbridge.h"
#include "TestInclude.h"
#include "MotorChannelsEnum.h"

namespace Hardware
{
class MotorController
{
private:
	IHbridge* hbridge;
public:
	MotorController(IHbridge* hbridge);
	virtual ~MotorController();

	TVIRTUAL void initialization();
	void turnOn(MOTOR_CHANNEL ch);
	void turnOff(MOTOR_CHANNEL ch);
	void forward(MOTOR_CHANNEL ch);
	void backward(MOTOR_CHANNEL ch);
	//void setVelocity(MOTOR_CHANNEL ch, uint32_t speed);
	void selfTest();
};
}

