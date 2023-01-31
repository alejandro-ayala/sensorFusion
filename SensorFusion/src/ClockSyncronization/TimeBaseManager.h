#pragma once

#include "ClockSyncronization/TimeController.h"
#include "Communication/ICommunication.h"
#include "TimeStamp.h"
#include "Conectivity/HTTPClient.h"

namespace ClockSyncronization
{
class TimeBaseManager
{
private:
	TimeController*                timeController;
	Communication::ICommunication* canController;
	Conectivity::HTTPClient*       httpClient;
	TimeStamp                      globalTimeStamp;
	TimeBaseRef                    globalTimeReference;
	uint8_t                        seqCounter;
	void sendSyncMessage();
	void sendFollowUpMessage();
	void syncTimeReference();
public:
	TimeBaseManager(TimeController* timecontroller, Communication::ICommunication* icomm, Conectivity::HTTPClient* httpclient);
	~TimeBaseManager();
	void initialization();
	TimeStamp getGlobalTime();
	TimeBaseRef getReferenceTime(cstring response);
	void updateGlobalTime(TimeStamp newTimeStamp);
	void sendGlobalTime();
};
}
