#include <hardware_abstraction/Devices/MotorControl/MotorControl.h>

namespace hardware_abstraction
{
namespace Devices
{
namespace MotorControl
{
MotorControl::MotorControl(IHbridge* hbridge): IController("MotorControl") , hbridge(hbridge)
{

}

MotorControl::~MotorControl() {}

void MotorControl::initialize()
{

}

void MotorControl::turnOn(MOTOR_CHANNEL ch)
{
	hbridge->turnOn(ch);
}

void MotorControl::turnOff(MOTOR_CHANNEL ch)
{
	hbridge->turnOff(ch);
}

void MotorControl::forward(MOTOR_CHANNEL ch)
{
	hbridge->turnOff(ch);
}

void MotorControl::backward(MOTOR_CHANNEL ch)
{
	hbridge->backward(ch);
}

//void MotorControl::setVelocity(MOTOR_CHANNEL ch, uint32_t speed)
//{
//	hbridge->setVelocity(ch,speed);
//}

bool MotorControl::selfTest()
{
	bool result;
	MOTOR_CHANNEL ch = MOTOR_CHANNEL::LEFT;
	turnOn(ch);
	turnOff(ch);
	turnOff(ch);
	backward(ch);
	return result;
}

}
}
}
