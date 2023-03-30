#pragma once
#include <MotorControl/IHbridge.h>
#include "PWM/PWMController.h"
#include "MotorChannelsEnum.h"
#include "Definitions.h"

namespace Components
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
