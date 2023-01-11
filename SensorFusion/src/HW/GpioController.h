#pragma once

#include "TestInclude.h"
#include "GpioInterface.h"
#include  "IController.h"

namespace Hardware
{
class GpioController : public IController
{
private:
	AxiGpioInterface gpioIntefaces;
public:
	GpioController(AxiGpioInterface& gpioIntefaces);
	virtual ~GpioController();

	TVIRTUAL void initialize() override;

	void writeGPIO(XGpio *InstancePtr, uint8_t channel, uint32_t mask);
	uint32_t readGPIO(XGpio *InstancePtr, uint8_t channel);
	void configGPIO(XGpio *InstancePtr, uint32_t gpio, uint8_t direction);

	void writePmodGPIO(PmodGpio *InstancePtr, uint8_t pinNumber, uint32_t value);
	uint32_t readPmodGPIO(PmodGpio *InstancePtr, uint8_t pinNumber);
	void configPmodGPIO(PmodGpio *InstancePtr,uint32_t address, uint8_t direction);

	bool selfTest() override;
};
}
