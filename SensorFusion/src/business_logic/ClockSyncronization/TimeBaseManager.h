#pragma once

#include <business_logic/ClockSyncronization/TimeController.h>
#include <business_logic/ClockSyncronization/TimeStamp.h>
//#include <business_logic/Conectivity/HTTPClient.h>
#include <hardware_abstraction/Controllers/CAN/PsCanController.h>
#include <memory>
namespace business_logic
{
namespace ClockSyncronization
{
class TimeBaseManager
{
private:
	std::shared_ptr<TimeController>timeController;
	std::shared_ptr<hardware_abstraction::Controllers::PsCanController>canController;
	//std::shared_ptr<Conectivity::HTTPClient>httpClient;
	TimeStamp                      globalTimeStamp;
	TimeBaseRef                    globalTimeReference;
	uint8_t                        seqCounter;
	bool sendSyncMessage();
	bool sendFollowUpMessage();
	void syncTimeReference();
public:
	TimeBaseManager(const std::shared_ptr<TimeController>& timecontroller, const std::shared_ptr<hardware_abstraction::Controllers::PsCanController>& icomm);//, const std::shared_ptr<Conectivity::HTTPClient>& httpclient);
	~TimeBaseManager();
	void initialization();
	TimeStamp getGlobalTime();
	TimeBaseRef parseReferenceTime(const std::string& response);
	void updateGlobalTime(TimeStamp newTimeStamp);
	void sendGlobalTime();
	uint64_t getAbsotuleTime() const;
	uint64_t getRelativeTime() const;
};
}
}
