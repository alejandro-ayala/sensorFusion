#include <business_logic/ClockSyncronization/TimeController.h>
#include <hardware_abstraction/Controllers/Timer/ZynqGT.h>
#include "sleep.h"
#include "services/Logger/LoggerMacros.h"

namespace business_logic
{
using namespace hardware_abstraction::Controllers;

namespace ClockSyncronization
{
TimeController::TimeController() : IController("TimeController")
{
	internalTimer = std::make_shared<ZynqGT>();
}

TimeController::~TimeController()
{
	LOG_DEBUG("TimeController::~TimeController");
};

void TimeController::initialize()
{
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

}

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

void TimeController::setGlobalTimeReference(const TimeStamp& gt)
{
	globalTimeReference = gt;
}

TimeStamp TimeController::getGlobalTimeReference()
{
	return globalTimeReference;
}

uint64_t TimeController::getLocalTime()
{
	uint64_t localNs = globalTimeReference.toNs() + getCurrentNsecTime();
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
