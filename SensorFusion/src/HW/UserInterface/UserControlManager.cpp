
#include "UserControlManager.h"
#include "GPIO/GpioInterface.h"
#include "Definitions.h"

namespace Hardware
{
XGpio gpio0;
XGpio gpio1;
uint32_t SPI_Address = 0x00;
//PmodGpio pmodGpioBtn;
PmodGpio pmodGpioLeds;
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

//	gpioInterface.axiLiteGpioBtn.gpioDriver = &pmodGpioBtn;
//	gpioInterface.axiLiteGpioBtn.address = XPAR_PMODGPIO_0_AXI_LITE_GPIO_BASEADDR;
//	gpioInterface.axiLiteGpioBtn.direction = 0xFF;

//	gpioInterface.axiLiteGpioLed.gpioDriver = &pmodGpioLeds;
//	gpioInterface.axiLiteGpioLed.address = XPAR_PMODGPIO_1_AXI_LITE_GPIO_BASEADDR;
//	gpioInterface.axiLiteGpioLed.direction = 0x00;

	gpioController   = new GpioController(gpioInterface);
	keypadController = new KeyPadController();

	oledController   = new OLEDController();

}

UserControlManager::~UserControlManager()
{
	delete gpioController;
}

void UserControlManager::initialization()
{
	gpioController->initialize();
	keypadController->initialize();
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

void UserControlManager::controlPmodLED(uint8_t ledChannel, bool state)
{
	gpioController->writePmodGPIO(gpioInterface.axiLiteGpioLed.gpioDriver,ledChannel, state);
}
bool UserControlManager::readPmodButton(uint8_t buttonChannel)
{
	return gpioController->readPmodGPIO(gpioInterface.axiLiteGpioBtn.gpioDriver,buttonChannel);
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

//	input = readPmodButton(1);
//	input = readPmodButton(2);
//	input = readPmodButton(3);
//	input = readPmodButton(4);

//	controlPmodLED(1,true);
//	controlPmodLED(2,true);
//	controlPmodLED(3,true);
//	controlPmodLED(4,true);
//
//	controlPmodLED(1,false);
//	controlPmodLED(2,false);
//	controlPmodLED(3,false);
//	controlPmodLED(4,false);

	keypadController->selfTest();

	oledController->selfTest();
}
}
