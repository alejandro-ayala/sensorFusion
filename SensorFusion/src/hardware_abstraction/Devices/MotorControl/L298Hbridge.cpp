#include <hardware_abstraction/Devices/MotorControl/L298Hbridge.h>
#include "services/Logger/LoggerMacros.h"
namespace hardware_abstraction
{
namespace Devices
{
namespace MotorControl
{
void L298Hbridge::initialize()
{
	pwmController->initialize();
}

void L298Hbridge::turnOn(MOTOR_CHANNEL ch)
{
	LOG_TRACE("\r\nL298Hbridge::turnOn\r\n");
	pwmController->enable();
}

void L298Hbridge::turnOff(MOTOR_CHANNEL ch)
{
	LOG_TRACE("\r\nL298Hbridge::turnOff\r\n");
	pwmController->disable();
}

void L298Hbridge::forward(MOTOR_CHANNEL ch)
{
	LOG_TRACE("\r\nL298Hbridge::forward\r\n");
}

void L298Hbridge::backward(MOTOR_CHANNEL ch)
{
	LOG_TRACE("\r\nL298Hbridge::backward\r\n");
}

}
}
}
