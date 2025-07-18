#include <hardware_abstraction/Controllers/Timer/ZynqTimer.h>
#include "services/Exception/SystemExceptions.h"

#include "xscutimer.h"
#include "xscugic.h"

namespace hardware_abstraction
{
namespace Controllers
{
#define timerDeviceId		XPAR_XSCUTIMER_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define TIMER_IRPT_INTR		XPAR_SCUTIMER_INTR
#define TIMER_LOAD_VALUE	0xFFFF
volatile int TimerExpired;
volatile bool elapseTime;
XScuTimer TimerInstance;	/* Cortex A9 Scu Private Timer Instance */
XScuGic IntcInstance;		/* Interrupt Controller Instance */
static void timerInterruptHandler(void *CallBackRef);
static void timerInterruptHandler(void *CallBackRef)
{
	XScuTimer *TimerInstancePtr = (XScuTimer *) CallBackRef;

	if (XScuTimer_IsExpired(TimerInstancePtr)) {
		XScuTimer_ClearInterruptStatus(TimerInstancePtr);
		TimerExpired++;
		if (TimerExpired == 3) {
			elapseTime = true;
			XScuTimer_DisableAutoReload(TimerInstancePtr);
		}
	}
}

ZynqTimer::ZynqTimer() : ITimer() {}

void ZynqTimer::initialize()
{
	int Status;

	int LastTimerExpired = 0;
	XScuTimer_Config *ConfigPtr;

	ConfigPtr = XScuTimer_LookupConfig(timerDeviceId);

	Status = XScuTimer_CfgInitialize(&TimerInstance, ConfigPtr,ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS)
	{
		THROW_CONTROLLERS_EXCEPTION(services::ControllersErrorId::TimerInitializationError, "Exception during XScuTimer_CfgInitialize");
	}

	Status = XScuTimer_SelfTest(&TimerInstance);
	if (Status != XST_SUCCESS)
	{
		THROW_CONTROLLERS_EXCEPTION(services::ControllersErrorId::TimerInitializationError, "Exception during XScuTimer_SelfTest");
	}

	XScuGic_Config *IntcConfig;

	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig)
	{
		THROW_CONTROLLERS_EXCEPTION(services::ControllersErrorId::TimerInitializationError, "Exception during XScuGic_LookupConfig");
	}

	Status = XScuGic_CfgInitialize(&IntcInstance, IntcConfig,	IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS)
	{
		THROW_CONTROLLERS_EXCEPTION(services::ControllersErrorId::TimerInitializationError, "Exception during XScuGic_CfgInitialize");
	}

	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,(Xil_ExceptionHandler)XScuGic_InterruptHandler,&IntcInstance);

}

void ZynqTimer::startTimer()
{
	elapseTime = false;

	int Status = XScuGic_Connect(&IntcInstance, TIMER_IRPT_INTR,(Xil_ExceptionHandler)timerInterruptHandler,(void *)&TimerInstance);
	if (Status != XST_SUCCESS)
	{
		THROW_CONTROLLERS_EXCEPTION(services::ControllersErrorId::TimerStartError, "Exception during XScuGic_Connect");
	}

	XScuGic_Enable(&IntcInstance, TIMER_IRPT_INTR);
	XScuTimer_EnableInterrupt(&TimerInstance);
	Xil_ExceptionEnable();

	XScuTimer_EnableAutoReload(&TimerInstance);
	XScuTimer_Start(&TimerInstance);
}

void ZynqTimer::stopTimer()
{
	XScuTimer_Stop(&TimerInstance);
	XScuGic_Disconnect(&IntcInstance, TIMER_IRPT_INTR);
}

void ZynqTimer::restartTimer()
{
	XScuTimer_RestartTimer(&TimerInstance);
}

void ZynqTimer::setPeriod(uint32_t tickPeriod)
{
	XScuTimer_LoadTimer(&TimerInstance, tickPeriod);
}

uint64_t ZynqTimer::getCurrentTicks()
{
	return XScuTimer_GetCounterValue(&TimerInstance);
}

double ZynqTimer::getCurrentSec()
{
	uint64_t currentTick = getCurrentTicks();
	return currentTick / countPerMicroSeconds;
}

double ZynqTimer::getCurrentUsec()
{
	uint64_t currentTick = getCurrentTicks();
	return currentTick / countPerMicroSeconds;
}

double ZynqTimer::getCurrentNsec()
{
	uint64_t currentTick = getCurrentTicks();
	return currentTick / countPerMicroSeconds;
}
bool ZynqTimer::elapsedTime()
{
	return elapseTime;
}
}
}
