#pragma once

#include <business_logic/ClockSyncronization/TimeController.h>
#include <business_logic/ClockSyncronization/TimeStamp.h>
#include <business_logic/Conectivity/HTTPClient.h>
#include <hardware_abstraction/Controllers/CAN/CanController.h>

namespace business_logic
{
namespace ClockSyncronization
{
class TimeBaseManager
{
private:
	TimeController*                timeController;
	hardware_abstraction::Controllers::CanController*       canController;
	Conectivity::HTTPClient*       httpClient;
	TimeStamp                      globalTimeStamp;
	TimeBaseRef                    globalTimeReference;
	uint8_t                        seqCounter;
	void sendSyncMessage();
	void sendFollowUpMessage();
	void syncTimeReference();
public:
	TimeBaseManager(TimeController* timecontroller, hardware_abstraction::Controllers::CanController* icomm, Conectivity::HTTPClient* httpclient);
	~TimeBaseManager();
	void initialization();
	TimeStamp getGlobalTime();
	TimeBaseRef getReferenceTime(cstring response);
	void updateGlobalTime(TimeStamp newTimeStamp);
	void sendGlobalTime();
};
}
}
