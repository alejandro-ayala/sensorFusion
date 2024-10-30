#pragma once

#include <hardware_abstraction/Controllers/IController.h>
#include <hardware_abstraction/Controllers/PWM/PWMController.h>
#include <memory>
namespace hardware_abstraction
{
namespace Devices
{
class ServoMotorControl : public Controllers::IController
{
private:
	std::shared_ptr<Controllers::PWMController> m_pwmController;
	uint8_t m_angle;
	const uint8_t m_maxAngle = 180;
	const uint8_t m_maxDutyCycle = 100;
public:
	ServoMotorControl(std::shared_ptr<Controllers::PWMController> pwmCtrl);
	virtual ~ServoMotorControl() = default;

	TVIRTUAL void initialize() override;

	void setAngle(uint8_t angle);
	uint8_t getAngle() const;
	bool selfTest() override;
};
}
}
