#pragma once
#include <hardware_abstraction/Controllers/IController.h>
#include "PWMConfig.h"
#include "TestInclude.h"

namespace hardware_abstraction
{
namespace Controllers
{

class PWMController : public IController
{
private:
	uint32_t m_pwmBaseAddr;
	uint32_t m_pwmFreq_Hz;
	uint8_t  m_pwmIndex;

public:
	PWMController(const PWMConfig& config = PWMConfig());
	TVIRTUAL ~PWMController();

	TVIRTUAL void initialize() override;
	TVIRTUAL void enable();
	TVIRTUAL void disable();
	TVIRTUAL void setDutyCicle(uint32_t dutyCycle);
	TVIRTUAL void setFrequency(uint32_t freq);
	TVIRTUAL uint32_t getDutyCicle();
	TVIRTUAL uint32_t getPeriod();
	bool selfTest() override;
};
}
}
