#pragma once

#include "TestInclude.h"
#include "GpioInterface.h"
namespace Hardware
{
class GpioController
{
private:
	AxiGpioInterface gpioIntefaces;
public:
	GpioController(AxiGpioInterface& gpioIntefaces);
	virtual ~GpioController();

	TVIRTUAL void initialization();

	void writeGPIO(XGpio *InstancePtr, uint8_t channel, uint32_t mask);
	uint32_t readGPIO(XGpio *InstancePtr, uint8_t channel);
	void configGPIO(XGpio *InstancePtr, uint32_t gpio, uint8_t direction);
	void selfTest();
};
}
