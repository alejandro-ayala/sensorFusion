#pragma once

#include <business_logic/ClockSyncronization/TimeController.h>
#include <business_logic/ClockSyncronization/TimeStamp.h>
#include <hardware_abstraction/Controllers/CAN/CanController.h>
#include <memory>

namespace business_logic
{
namespace ClockSyncronization
{
class SharedClockSlaveManager
{
private:
	std::shared_ptr<TimeController> timeController;
	std::shared_ptr<hardware_abstraction::Controllers::CanController> canController;
	TimeStamp                      globalTimeStamp;
	TimeBaseRef                    globalTimeReference;
public:
	SharedClockSlaveManager(const std::shared_ptr<TimeController>& timecontroller, const std::shared_ptr<hardware_abstraction::Controllers::CanController> cancontroller);
	~SharedClockSlaveManager();
	void initialization();
	bool getGlobalTime();
	TimeStamp getTimeReference() const;
	uint64_t getLocalTime() const;
};
}
}
