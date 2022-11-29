#pragma once
#include "xgpio.h"

namespace Hardware
{
#define LED_CHANNEL 1
#define BTN_CHANNEL 2
#define SWITCH_CHANNEL 1
#define RGB_CHANNEL 2

typedef struct GPIOcontrol
{
	XGpio*   gpioDriver;
	uint32_t chInput;
	uint32_t chOutput;
	uint32_t directionInput;
	uint32_t directionOutput;
} GPIOcontrol;

typedef struct PmodGpio
{
	uint32_t GPIO_addr;
	uint32_t cpuClockFreqHz;
} PmodGpio;

typedef struct PmodGPIOcontrol
{
	PmodGpio*   gpioDriver;
	uint32_t    address;
	bool        input;
	uint32_t    direction;
} PmodGPIOcontrol;

typedef struct
{
	GPIOcontrol     axiGpio0;
	GPIOcontrol     axiGpio1;
	PmodGPIOcontrol axiLiteGpioLed;
	PmodGPIOcontrol axiLiteGpioBtn;
}AxiGpioInterface;
}