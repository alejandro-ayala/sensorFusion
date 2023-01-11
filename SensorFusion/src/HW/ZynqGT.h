#pragma once
#include "xil_types.h"
#include "xparameters.h"
#include <cstdint>
#include "ClockSyncronization/ITimer.h"

namespace Hardware
{
class ZynqGT : public ClockSyncronization::ITimer
{
private:
	static constexpr uint64_t countPerSeconds = (XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ /2); /* GT is clocked at 1/2 of the CPU frequency */
	static constexpr uint64_t countPerMicroSeconds = (XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ / (2U*1000000U));
	static constexpr double countPerNanoSeconds = 0.33;
public:
	ZynqGT();
	virtual ~ZynqGT();
	uint64_t currentTick;

	void startTimer() override;
	void stopTimer()  override;
	void restartTimer() override;
	uint64_t getCurrentTicks() override;
	double getCurrentSec() override;
	double getCurrentUsec() override;
	double getCurrentNsec() override;
};
}
