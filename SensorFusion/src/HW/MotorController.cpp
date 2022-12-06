#include "MotorController.h"

namespace Hardware
{
MotorController::MotorController(IHbridge* hbridge): IController("MotorController") , hbridge(hbridge)
{

}

MotorController::~MotorController() {}

void MotorController::initialize()
{

}

void MotorController::turnOn(MOTOR_CHANNEL ch)
{
	hbridge->turnOn(ch);
}

void MotorController::turnOff(MOTOR_CHANNEL ch)
{
	hbridge->turnOff(ch);
}

void MotorController::forward(MOTOR_CHANNEL ch)
{
	hbridge->turnOff(ch);
}

void MotorController::backward(MOTOR_CHANNEL ch)
{
	hbridge->backward(ch);
}

//void MotorController::setVelocity(MOTOR_CHANNEL ch, uint32_t speed)
//{
//	hbridge->setVelocity(ch,speed);
//}

void MotorController::selfTest()
{
	MOTOR_CHANNEL ch = MOTOR_CHANNEL::LEFT;
	turnOn(ch);
	turnOff(ch);
	turnOff(ch);
	backward(ch);
}

}
