#pragma once
#include "GPIO/GpioController.h"
#include "KeyPadController.h"
#include "OLED/OLEDController.h"
#include "SPI/SPIController.h"
#include "TestInclude.h"

namespace Components
{
namespace UserInterface
{
class UserControlManager
{
private:
	Controllers::GpioController*    gpioController;
	KeyPadController*  keypadController;
	Controllers::AxiGpioInterface   gpioInterface;
	Controllers::OLEDController*    oledController;
	Controllers::SPIController*     spiController;
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
}
