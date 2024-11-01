#include <hardware_abstraction/Controllers/PWM/PWMController.h>
#include "xil_types.h"
#include "xstatus.h"
#include "xil_io.h"

namespace hardware_abstraction
{
namespace Controllers
{
#define PWM_AXI_CTRL_REG_OFFSET 0
#define PWM_AXI_PERIOD_REG_OFFSET 8
#define PWM_AXI_DUTY_REG_OFFSET 64
#define PWM_CLK_INPUT 50000000
#define PWM_CHANNELS 4

PWMController::PWMController(const PWMConfig& config) : IController("PWMController"), m_pwmBaseAddr(XPAR_PWM_MOTOR_PWM_AXI_BASEADDR), m_pwmFreq_Hz(config.pwmFreq), m_pwmIndex(config.pwmIndex)
{

}

PWMController::~PWMController()
{
	disable();
}

void PWMController::initialize()
{
	enable();
	setFrequency(m_pwmFreq_Hz);
	setDutyCicle(50);
}

void PWMController::enable()
{
	Xil_Out32(m_pwmBaseAddr + PWM_AXI_CTRL_REG_OFFSET, 1);
}

void PWMController::disable()
{
	Xil_Out32(m_pwmBaseAddr + PWM_AXI_CTRL_REG_OFFSET, 0);
}

void PWMController::setDutyCicle(uint32_t dutyCycle)
{
	//uint32_t countClk = dutyCycle * PWM_CLK_INPUT / (m_pwmFreq_Hz * 100);
	if(dutyCycle == 100)
		dutyCycle = 99;
	uint32_t countClk = (PWM_CLK_INPUT / m_pwmFreq_Hz) * dutyCycle / 100;
	Xil_Out32(m_pwmBaseAddr + PWM_AXI_DUTY_REG_OFFSET + (PWM_CHANNELS*m_pwmIndex), countClk);
}

void PWMController::setFrequency(uint32_t freq)
{
	m_pwmFreq_Hz = freq;
	uint32_t countClk = PWM_CLK_INPUT / m_pwmFreq_Hz;
	Xil_Out32(m_pwmBaseAddr + PWM_AXI_PERIOD_REG_OFFSET, countClk);
}

uint32_t PWMController::getDutyCicle()
{
	return Xil_In32(m_pwmBaseAddr + PWM_AXI_DUTY_REG_OFFSET + (PWM_CHANNELS * m_pwmIndex));
}

uint32_t PWMController::getPeriod()
{
	return Xil_In32(m_pwmBaseAddr + PWM_AXI_PERIOD_REG_OFFSET);
}

bool PWMController::selfTest()
{
	bool result = true;
	return result;
}
}
}
