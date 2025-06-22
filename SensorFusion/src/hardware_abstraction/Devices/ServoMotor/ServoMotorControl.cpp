#include "ServoMotorControl.h"
#include "services/Logger/LoggerMacros.h"
namespace hardware_abstraction
{
namespace Devices
{
ServoMotorControl::ServoMotorControl(std::unique_ptr<Controllers::PWMController> pwmCtrl): IController("ServoMotorControl"), m_pwmController(std::move(pwmCtrl)), m_angle(0), m_initialized(false)
{

}

void ServoMotorControl::initialize()
{
	if(!m_initialized)
	{
		selfTest();
		m_pwmController->initialize();
		m_initialized = true;
	}
	else
	{
		LOG_WARNING("ServoMotorControl already initialized");
	}

}

void ServoMotorControl::setAngle(uint8_t angle)
{
//	m_angle = angle;
//	uint8_t dutyCycle = m_maxDutyCycle * angle / m_maxAngle;
//	m_pwmController->setDutyCicle(/*dutyCycle*/angle);
    if (angle > m_maxAngle)
        angle = m_maxAngle;
    float dutyCycle = 100.0f - (m_minPwmAngle + (static_cast<float>(angle) * (m_maxPwmAngle - m_minPwmAngle) / m_maxAngle));

    m_pwmController->setDutyCicle(dutyCycle);
}

uint8_t ServoMotorControl::getAngle() const
{
	return m_angle;
}

bool ServoMotorControl::selfTest()
{
	bool result;

	return result;
}
}
}
