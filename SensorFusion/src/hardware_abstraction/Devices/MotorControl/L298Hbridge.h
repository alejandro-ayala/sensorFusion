#pragma once
#include <hardware_abstraction/Controllers/PWM/PWMController.h>
#include <hardware_abstraction/Devices/MotorControl/IHbridge.h>
#include "MotorChannelsEnum.h"
#include "Definitions.h"

namespace hardware_abstraction
{
namespace Devices
{
namespace MotorControl
{
class L298Hbridge : public IHbridge
{
private:
	Controllers::PWMController* pwmController;
public:
	L298Hbridge(Controllers::PWMController* pwmcontroller) : IHbridge("L298Hbridge") , pwmController (pwmcontroller){};
	~L298Hbridge(){};

	void initialize();
	void turnOn(MOTOR_CHANNEL ch) override;
	void turnOff(MOTOR_CHANNEL ch) override;
	void forward(MOTOR_CHANNEL ch) override;
	void backward(MOTOR_CHANNEL ch) override;
	//void setVelocity(MOTOR_CHANNEL ch, uint32_t speed) override;
};
}
}
}
