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
	std::unique_ptr<Controllers::PWMController> m_pwmController;
	uint8_t m_angle;
    float m_minPwmAngle = 5.0;
    float m_maxPwmAngle = 12.5;
    float m_maxAngle = 180.0;
	bool m_initialized;
public:
	ServoMotorControl(std::unique_ptr<Controllers::PWMController> pwmCtrl);
	virtual ~ServoMotorControl() = default;

	void initialize() override;

	void setAngle(uint8_t angle);
	uint8_t getAngle() const;
	bool selfTest() override;
};
}
}
