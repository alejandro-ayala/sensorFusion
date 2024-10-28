#pragma once
#include <hardware_abstraction/Controllers/Timer/ITimer.h>
#include <cstdint>

namespace hardware_abstraction
{
namespace Controllers
{
class ZynqTimer : public ITimer
{
private:
	static constexpr uint64_t countPerSeconds = 0;
	static constexpr uint64_t countPerMicroSeconds = 0;
	static constexpr uint64_t countPerNanoSeconds = 0;
public:
	ZynqTimer();
	virtual ~ZynqTimer();

	void configure();
	void startTimer() override;
	void stopTimer()  override;
	void restartTimer() override;
	void setPeriod(uint32_t period);
	uint64_t getCurrentTicks() override;
	bool elapsedTime();
	double getCurrentSec() override;
	double getCurrentUsec() override;
	double getCurrentNsec() override;
};
}
}
