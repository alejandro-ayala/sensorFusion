#pragma once
#include "GpioController.h"
#include "KeyPadController.h"
#include "TestInclude.h"
namespace Hardware
{

class UserControlManager
{
private:
	GpioController*    gpioController;
	KeyPadController*  keypadController;
	AxiGpioInterface   gpioInterface;
public:
	UserControlManager();
	virtual ~UserControlManager();

	TVIRTUAL void initialization();

	void controlRGB(uint32_t color, bool state);
	void controlLED(uint8_t ledChannel, bool state);
	bool readButton(uint8_t buttonChannel);
	bool readSwitch(uint8_t switchChannel);
	void selfTest();

	void controlPmodLED(uint8_t ledChannel, bool state);
	bool readPmodButton(uint8_t buttonChannel);
};
}
