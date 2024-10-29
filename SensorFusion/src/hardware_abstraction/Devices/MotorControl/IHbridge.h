#pragma once
#include "MotorChannelsEnum.h"
#include "Definitions.h"

namespace hardware_abstraction
{
namespace Devices
{
namespace MotorControl
{
class IHbridge {
private:
	cstring name;
public:
	IHbridge(cstring name) : name(name){};
	virtual ~IHbridge(){};

	virtual void turnOn(MOTOR_CHANNEL ch) = 0;
	virtual void turnOff(MOTOR_CHANNEL ch) = 0;
	virtual void forward(MOTOR_CHANNEL ch) = 0;
	virtual void backward(MOTOR_CHANNEL ch) = 0;
	//virtual void setVelocity(MOTOR_CHANNEL ch, uint32_t speed) = 0;
};
}
}
}
