#pragma once
#include "CAN/CanController.h"
#include "IData.h"
#include "ClockSyncronization/TimeStamp.h"
#include "ClockSyncronization/TimeController.h"

namespace Components
{
namespace Communication
{
class CommunicationManager
{
private:
	Controllers::CanController*             canController;
	ClockSyncronization::TimeController* timeController;
	ClockSyncronization::TimeStamp       globalTimeStamp;
public:
	CommunicationManager(ClockSyncronization::TimeController* timecontroller, Controllers::CanController* cancontroller);
	virtual ~CommunicationManager();

	void initialization();
	void sendData(IData msg);
	IData receiveData(void);
	bool selfTest();

};
}
}
