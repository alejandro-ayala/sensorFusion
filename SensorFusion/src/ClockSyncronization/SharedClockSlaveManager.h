#pragma once

#include "ClockSyncronization/TimeController.h"
#include "Communication/ICommunication.h"
#include "TimeStamp.h"

namespace ClockSyncronization
{
class SharedClockSlaveManager
{
private:
	TimeController*                timeController;
	Communication::ICommunication* canController;
	TimeStamp                      globalTimeStamp;
	TimeBaseRef                    globalTimeReference;
public:
	SharedClockSlaveManager(TimeController* timecontroller, Communication::ICommunication* icomm);
	~SharedClockSlaveManager();
	void initialization();
	bool getGlobalTime();
	TimeStamp getTimeReference() const;
};
}
