#include <ClockSyncronization/TimeController.h>
#include "HW/ZynqGT.h"
#include "sleep.h"

using namespace Hardware;
namespace ClockSyncronization
{
TimeController::TimeController() : IController("TimeController")
{
	internalTimer = new ZynqGT();
}

TimeController::~TimeController() {};

void TimeController::initialize()
{
	restartTimer();
	initialized = true;
}

void TimeController::startTimer()
{
	internalTimer->startTimer();
}
void TimeController::stopTimer()
{
	internalTimer->stopTimer();
}

void TimeController::restartTimer()
{
	internalTimer->restartTimer();
}
//void TimeController::setPeriod(uint32_t period)
//{
//	internalTimer->setPeriod(period);
//}

double TimeController::getCurrentSecTime()
{
	return internalTimer->getCurrentSec();
}

double TimeController::getCurrentNsecTime()
{
	return internalTimer->getCurrentNsec();

}
uint64_t TimeController::getCurrentTicks()
{
	return internalTimer->getCurrentTicks();
}
//bool TimeController::elapsedTime()
//{
//	return internalTimer->elapsedTime();
//}

bool TimeController::selfTest()
{
	bool elapsed = false;
	double elapsedTime, currentNs,currentNs2 ;
	internalTimer->restartTimer();
	currentNs   = internalTimer->getCurrentNsec();
	while(1)
	{
		if(elapsed==true)
		{
			internalTimer->stopTimer();
			elapsed = false;
			currentNs2   = internalTimer->getCurrentNsec();
			internalTimer->restartTimer();
			currentNs   = internalTimer->getCurrentNsec();

		}

	}

}
}
