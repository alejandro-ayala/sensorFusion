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
	uint64_t currentTick;
	XTime_GetTime(&currentTick);
	return currentTick;
}

double ZynqGT::getCurrentSec()
{
	uint64_t currentTick = getCurrentTicks();
	static constexpr double tickDurationSec =  static_cast<double>(GT_Prescaler) / static_cast<double>(XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ);
	auto currentSec = static_cast<double>(currentTick) * tickDurationSec;
	return currentSec;
}

double ZynqGT::getCurrentUsec()
{
	auto currentTick = getCurrentTicks();
	static constexpr double tickDurationUs = (static_cast<double>(GT_Prescaler) / static_cast<double>(XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ)) * 1e6;
	auto currentUs = static_cast<double>(currentTick) * tickDurationUs;
	return currentUs;
}

double ZynqGT::getCurrentNsec()
{
	auto currentTick = getCurrentTicks();
	static constexpr double tickDurationNs = (static_cast<double>(GT_Prescaler) / static_cast<double>(XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ)) * 1e9;
	auto currentNs = static_cast<double>(currentTick) * tickDurationNs;
	return currentNs;
}
}
}
