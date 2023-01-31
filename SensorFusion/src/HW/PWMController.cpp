#include "PWMController.h"
#include "xil_types.h"
#include "xstatus.h"
#include "xil_io.h"

namespace Hardware
{
#define PWM_AXI_CTRL_REG_OFFSET 0
#define PWM_AXI_PERIOD_REG_OFFSET 8
#define PWM_AXI_DUTY_REG_OFFSET 64

PWMController::PWMController() : IController("PWMController"), baseAddr(XPAR_PWM_MOTOR_PWM_AXI_BASEADDR){};
PWMController::~PWMController()
{
	disable();
};

void PWMController::initialize()
{
	enable();
	setDutyCicle(1000, 1);
	setPeriod(10);
}

void PWMController::enable()
{
	Xil_Out32(baseAddr + PWM_AXI_CTRL_REG_OFFSET, 1);
}

void PWMController::disable()
{
	Xil_Out32(baseAddr + PWM_AXI_CTRL_REG_OFFSET, 0);
}

void PWMController::setDutyCicle(uint32_t clocks, uint32_t pwmIndex)
{
	Xil_Out32(baseAddr + PWM_AXI_DUTY_REG_OFFSET + (4*pwmIndex), clocks);
}

void PWMController::setPeriod(uint32_t clocks)
{
	Xil_Out32(baseAddr + PWM_AXI_PERIOD_REG_OFFSET, clocks);
}

uint32_t PWMController::getDutyCicle(uint32_t pwmIndex)
{
	return Xil_In32(baseAddr + PWM_AXI_DUTY_REG_OFFSET + (4*pwmIndex));
}

uint32_t PWMController::getPeriod()
{
	return Xil_In32(baseAddr + PWM_AXI_PERIOD_REG_OFFSET);
}

bool PWMController::selfTest()
{
	bool result = true;
	return result;
}
}
