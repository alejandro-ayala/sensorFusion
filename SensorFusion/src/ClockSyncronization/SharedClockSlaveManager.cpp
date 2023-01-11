#include <ClockSyncronization/SharedClockSlaveManager.h>

namespace ClockSyncronization
{
SharedClockSlaveManager::SharedClockSlaveManager(TimeController* timecontroller, Communication::ICommunication* icomm) : timeController(timecontroller), canController(icomm)
{

}

SharedClockSlaveManager::~SharedClockSlaveManager() {}

void SharedClockSlaveManager::initialize()
{

}

}
