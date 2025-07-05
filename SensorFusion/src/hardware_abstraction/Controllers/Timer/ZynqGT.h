#pragma once
#include <hardware_abstraction/Controllers/Timer/ITimer.h>
#include "xil_types.h"
#include "xparameters.h"
#include <cstdint>

namespace hardware_abstraction
{
namespace Controllers
{
class ZynqGT : public ITimer
{
private:
	static constexpr uint8_t GT_Prescaler = 2; /* GT is clocked at 1/2 of the CPU frequency */
public:
	ZynqGT();
	virtual ~ZynqGT();

	void startTimer() override;
	void stopTimer()  override;
	uint64_t getCurrentTicks() override;
	void restartTimer() override;
	double getCurrentSec() override;
	double getCurrentUsec() override;
	double getCurrentNsec() override;
};
}
}
