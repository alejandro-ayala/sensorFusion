#include "ServoMotorControl.h"

namespace hardware_abstraction
{
namespace Devices
{
ServoMotorControl::ServoMotorControl(std::shared_ptr<Controllers::PWMController> pwmCtrl): IController("ServoMotorControl"), m_pwmController(pwmCtrl), m_angle(0)
{

}

void ServoMotorControl::initialize()
{
	selfTest();
	m_pwmController->initialize();
}

void ServoMotorControl::setAngle(uint8_t angle)
{
//	m_angle = angle;
//	uint8_t dutyCycle = m_maxDutyCycle * angle / m_maxAngle;
//	m_pwmController->setDutyCicle(/*dutyCycle*/angle);
    // Asegúrate de que el ángulo esté entre 0 y 180 (para evitar valores fuera de rango)
    if (angle > 180)
        angle = 180;

    // Calcular el duty cycle correspondiente al ángulo (de 5% a 10%)
    float dutyCycle = 5.0f + (static_cast<float>(angle) * 5.0f / 180.0f);

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
