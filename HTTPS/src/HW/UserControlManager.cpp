
#include "UserControlManager.h"
#include "GpioInterface.h"
#include "Definitions.h"

XGpio gpio0;
XGpio gpio1;
namespace Hardware
{

UserControlManager::UserControlManager()
{
	gpioInterface.axiGpio0.gpioDriver = &gpio0;
	gpioInterface.axiGpio0.chInput  = BTN_CHANNEL;
	gpioInterface.axiGpio0.chOutput = LED_CHANNEL;
	gpioInterface.axiGpio0.directionInput  = 0x0F;
	gpioInterface.axiGpio0.directionOutput = 0x00;
	gpioInterface.axiGpio1.gpioDriver = &gpio1;
	gpioInterface.axiGpio1.chInput  = SWITCH_CHANNEL;
	gpioInterface.axiGpio1.chOutput = RGB_CHANNEL;
	gpioInterface.axiGpio1.directionInput  = 0x0F;
	gpioInterface.axiGpio1.directionOutput = 0x00;
	gpioController = new GpioController(gpioInterface);
}

UserControlManager::~UserControlManager()
{
	delete gpioController;
}

void UserControlManager::initialization()
{
	gpioController->initialization();
}

void UserControlManager::controlRGB(uint32_t color, bool state)
{
	uint32_t colorAux = color && state;

	gpioController->writeGPIO(gpioInterface.axiGpio1.gpioDriver,
			gpioInterface.axiGpio1.chOutput, colorAux);
}

void UserControlManager::controlLED(uint8_t ledChannel, bool state)
{
	uint32_t mask = gpioController->readGPIO(gpioInterface.axiGpio0.gpioDriver,
			gpioInterface.axiGpio0.chOutput);

	if(state)
		mask |=  (1 << ledChannel);
	else
		mask &=  ~(1 << ledChannel);

	gpioController->writeGPIO(gpioInterface.axiGpio0.gpioDriver,
			gpioInterface.axiGpio0.chOutput, mask);
}

bool UserControlManager::readButton(uint8_t buttonChannel)
{
	auto regStatus = gpioController->readGPIO(gpioInterface.axiGpio0.gpioDriver,
			gpioInterface.axiGpio0.chInput);

	return ((regStatus >> buttonChannel) && 0x01);
}

bool UserControlManager::readSwitch(uint8_t switchChannel)
{
	auto regStatus = gpioController->readGPIO(gpioInterface.axiGpio1.gpioDriver,
			gpioInterface.axiGpio1.chInput);

	return ((regStatus >> switchChannel) && 0x01);
}

void UserControlManager::selfTest()
{
	bool input;
	controlLED(0,TURN_ON);
	controlLED(1,TURN_ON);
	controlLED(2,TURN_ON);
	controlLED(3,TURN_ON);

	controlLED(0,TURN_OFF);
	controlLED(1,TURN_OFF);
	controlLED(2,TURN_OFF);
	controlLED(3,TURN_OFF);

	input = readButton(0);
	input = readButton(1);
	input = readButton(2);
	input = readButton(3);

	input = readSwitch(0);
	input = readSwitch(1);
	input = readSwitch(2);
	input = readSwitch(3);

	controlRGB(0x0F,TURN_ON);

	controlRGB(0x0F,TURN_OFF);
}
}
