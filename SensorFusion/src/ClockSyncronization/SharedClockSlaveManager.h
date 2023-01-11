#pragma once

#include "ClockSyncronization/TimeController.h"
#include "Communication/ICommunication.h"

namespace ClockSyncronization
{
class SharedClockSlaveManager
{
private:
	TimeController*                timeController;
	Communication::ICommunication* canController;
public:
	SharedClockSlaveManager(TimeController* timecontroller, Communication::ICommunication* icomm);
	~SharedClockSlaveManager();
	void initialize();
};
}
