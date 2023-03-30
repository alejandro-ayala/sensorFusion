#pragma once
#include "Controllers/IController.h"
#include "Timer/ITimer.h"
#include "TimeStamp.h"

namespace Components
{
namespace ClockSyncronization
{
class TimeController : public Controllers::IController
{
private:
	Controllers::ITimer* internalTimer;
	TimeStamp globalTimeStamp;
public:
	TimeController();
	~TimeController();

	void initialize() override;
	bool selfTest()   override;

	bool elapsedTime();
	void startTimer();
	void stopTimer();
	void restartTimer();
	void setPeriod(uint32_t period);
	double getCurrentSecTime();
	double getCurrentNsecTime();
	uint64_t getCurrentTicks();
	void setGlobalTimeReference(const TimeStamp& gt);
	TimeStamp getGlobalTimeReference();
	uint64_t getLocalTime();
};
}
}
