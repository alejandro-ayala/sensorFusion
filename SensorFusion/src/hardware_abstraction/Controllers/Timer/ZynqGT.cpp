#include <hardware_abstraction/Controllers/Timer/ZynqGT.h>
#include "xtime_l.h"
#include "xil_io.h"
#include "services/Logger/LoggerMacros.h"

namespace hardware_abstraction
{
namespace Controllers
{

ZynqGT::ZynqGT() : ITimer() {}

ZynqGT::~ZynqGT()
{
	LOG_DEBUG("TimeController::~TimeController");
}

void ZynqGT::startTimer()
{
	Xil_Out32((u32)GLOBAL_TMR_BASEADDR + (u32)GTIMER_CONTROL_OFFSET, (u32)0x1);
}

void ZynqGT::stopTimer()
{
	Xil_Out32((u32)GLOBAL_TMR_BASEADDR + (u32)GTIMER_CONTROL_OFFSET, (u32)0x0);
}

void ZynqGT::restartTimer()
{
	XTime_SetTime(0);
}

uint64_t ZynqGT::getCurrentTicks()
{
	return currentTick;
}

double ZynqGT::getCurrentSec()
{
	uint64_t currentTick = getCurrentTicks();
	return currentTick / countPerSeconds;
}

double ZynqGT::getCurrentUsec()
{
	uint64_t currentTick = getCurrentTicks();
	return currentTick / countPerMicroSeconds;
}

double ZynqGT::getCurrentNsec()
{
	XTime_GetTime(&currentTick);
	return currentTick / countPerNanoSeconds;
}
}
}
