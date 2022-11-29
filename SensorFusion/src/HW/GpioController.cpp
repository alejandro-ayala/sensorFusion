
#include "GpioController.h"

#include "xil_io.h"
#include "xil_types.h"
#include "xparameters.h"
#include "xstatus.h"

namespace Hardware
{

GpioController::GpioController(AxiGpioInterface& gpioIntefaces) : gpioIntefaces(gpioIntefaces)
{

}

GpioController::~GpioController()
{

}

void  GpioController::initialization()
{

	int status = XGpio_Initialize(gpioIntefaces.axiGpio0.gpioDriver, XPAR_GPIO_0_DEVICE_ID);
	if(status != XST_SUCCESS)
	{
		//Throw exception
	}

	status = XGpio_Initialize(gpioIntefaces.axiGpio1.gpioDriver, XPAR_GPIO_1_DEVICE_ID);
	if(status != XST_SUCCESS)
	{
		//Throw exception
	}

	configGPIO(gpioIntefaces.axiGpio0.gpioDriver, gpioIntefaces.axiGpio0.chOutput, gpioIntefaces.axiGpio0.directionOutput);
	configGPIO(gpioIntefaces.axiGpio0.gpioDriver, gpioIntefaces.axiGpio0.chInput , gpioIntefaces.axiGpio0.directionInput);
	configGPIO(gpioIntefaces.axiGpio1.gpioDriver, gpioIntefaces.axiGpio1.chOutput, gpioIntefaces.axiGpio1.directionOutput);
	configGPIO(gpioIntefaces.axiGpio1.gpioDriver, gpioIntefaces.axiGpio1.chInput , gpioIntefaces.axiGpio1.directionInput);

	configPmodGPIO(gpioIntefaces.axiLiteGpioBtn.gpioDriver, gpioIntefaces.axiLiteGpioBtn.address, gpioIntefaces.axiLiteGpioBtn.direction);
	configPmodGPIO(gpioIntefaces.axiLiteGpioLed.gpioDriver, gpioIntefaces.axiLiteGpioLed.address, gpioIntefaces.axiLiteGpioLed.direction);

}

void GpioController::configGPIO(XGpio *InstancePtr, uint32_t channel, uint8_t direction)
{
	XGpio_SetDataDirection(InstancePtr, channel, direction);
}
uint32_t GpioController::readGPIO(XGpio *InstancePtr, uint8_t channel)
{
	return XGpio_DiscreteRead(InstancePtr, channel);
}
void GpioController::writeGPIO(XGpio *InstancePtr, uint8_t pinNumber, uint32_t mask)
{
	XGpio_DiscreteWrite(InstancePtr, pinNumber, mask);
}

void GpioController::configPmodGPIO(PmodGpio *InstancePtr,uint32_t address, uint8_t direction)
{
	InstancePtr->GPIO_addr = address;
	// 0b1111 for input 0b0000 for output, 0b0001 for pin1 in pin 2 out etc.
	Xil_Out32(InstancePtr->GPIO_addr + 4, direction);
}
uint32_t GpioController::readPmodGPIO(PmodGpio *InstancePtr, uint8_t pinNumber)
{
	uint32_t data = 0;
	data = Xil_In32(InstancePtr->GPIO_addr);
	data = data & (1 << (pinNumber - 1));

	if (data)
	  return 1;
	else
	  return 0;
}
void GpioController::writePmodGPIO(PmodGpio *InstancePtr, uint8_t pinNumber, uint32_t value)
{
	uint32_t data = 0;

	data = Xil_In32(InstancePtr->GPIO_addr);

	if (value != 0)
		Xil_Out32(InstancePtr->GPIO_addr, (data | (1 << (pinNumber - 1))));
	else
	  Xil_Out32(InstancePtr->GPIO_addr, (data & ~(1 << (pinNumber - 1))));
}

void GpioController::selfTest()
{
//
//	writeGPIO(&Gpio0, LED_CHANNEL, 0x0F);
//	uint32_t value = readGPIO(&Gpio0,BTN_CHANNEL);
//	xil_printf("\r\nselfTest buttons:%d",value);
//	/* Wait a small amount of time so the LED is visible */
//	for (int Delay = 0; Delay < 10000000; Delay++);
//	/* Clear the LED bit */
//	XGpio_DiscreteClear(&Gpio0, LED_CHANNEL, 0x0F);
//	/* Wait a small amount of time so the LED is visible */
}
}
