#pragma once
#include <business_logic/ClockSyncronization/TimeController.h>
#include <business_logic/ClockSyncronization/TimeStamp.h>
#include <business_logic/Communication/IData.h>
#include <hardware_abstraction/Controllers/CAN/CanController.h>
#include <memory>
#include <vector>
#include "CanMsg.h"

namespace business_logic
{
namespace Communication
{
class CommunicationManager
{
private:
	std::shared_ptr<ClockSyncronization::TimeController> timeController;
	std::shared_ptr<hardware_abstraction::Controllers::CanController> canController;
	ClockSyncronization::TimeStamp globalTimeStamp;
public:
	CommunicationManager(const std::shared_ptr<ClockSyncronization::TimeController>& timecontroller, const std::shared_ptr<hardware_abstraction::Controllers::CanController>& cancontroller);
	virtual ~CommunicationManager();

	void initialization();
	void sendData(IData msg);
	void sendData(const std::vector<business_logic::Communication::CanMsg>& dataToSend);
	IData receiveData(void);
	bool selfTest();

};
}
}
