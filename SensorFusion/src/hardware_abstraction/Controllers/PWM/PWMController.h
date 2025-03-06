#pragma once
#include <hardware_abstraction/Controllers/IController.h>
#include "PWMConfig.h"

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
	PWMController(const PWMConfig& config);
	~PWMController();

	void initialize() override;
	void enable();
	void disable();
	void setDutyCicle(float dutyCycle);
	void setFrequency(uint32_t freq);
	uint32_t getDutyCicle();
	uint32_t getPeriod();
	void setTicksDutyCicle(uint32_t dutyCycle);
	bool selfTest() override;
};
}
}
