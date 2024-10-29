#include <business_logic/ClockSyncronization/TimeController.h>
#include <hardware_abstraction/Controllers/Timer/ZynqGT.h>
#include "sleep.h"


namespace business_logic
{
using namespace hardware_abstraction::Controllers;

namespace ClockSyncronization
{
TimeController::TimeController() : IController("TimeController")
{
	internalTimer = new ZynqGT();
}

TimeController::~TimeController()
{
	delete internalTimer;
};

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
void TimeController::setGlobalTimeReference(const TimeStamp& gt)
{
	globalTimeStamp = gt;
	restartTimer();
}

TimeStamp TimeController::getGlobalTimeReference()
{
	return globalTimeStamp;
}

uint64_t TimeController::getLocalTime()
{
	uint64_t localNs = globalTimeStamp.toNs() + getCurrentNsecTime();
	return localNs;
}

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
}
