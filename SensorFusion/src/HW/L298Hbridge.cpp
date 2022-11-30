#include "L298Hbridge.h"

namespace Hardware
{
void L298Hbridge::turnOn(MOTOR_CHANNEL ch)
{
	xil_printf("\r\nL298Hbridge::turnOn\r\n");
}

void L298Hbridge::turnOff(MOTOR_CHANNEL ch)
{
	xil_printf("\r\nL298Hbridge::turnOff\r\n");
}

void L298Hbridge::forward(MOTOR_CHANNEL ch)
{
	xil_printf("\r\nL298Hbridge::forward\r\n");
}

void L298Hbridge::backward(MOTOR_CHANNEL ch)
{
	xil_printf("\r\nL298Hbridge::backward\r\n");
}

}
