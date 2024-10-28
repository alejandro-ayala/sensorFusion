#pragma once
#include <business_logic/ClockSyncronization/TimeController.h>
#include <business_logic/ClockSyncronization/TimeStamp.h>
#include <business_logic/Communication/IData.h>
#include <hardware_abstraction/Controllers/CAN/CanController.h>

namespace business_logic
{
namespace Communication
{
class CommunicationManager
{
private:
	hardware_abstraction::Controllers::CanController*             canController;
	ClockSyncronization::TimeController* timeController;
	ClockSyncronization::TimeStamp       globalTimeStamp;
public:
	CommunicationManager(ClockSyncronization::TimeController* timecontroller, hardware_abstraction::Controllers::CanController* cancontroller);
	virtual ~CommunicationManager();

	void initialization();
	void sendData(IData msg);
	IData receiveData(void);
	bool selfTest();

};
}
}
