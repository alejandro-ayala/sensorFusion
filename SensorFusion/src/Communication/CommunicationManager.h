#pragma once
#include "ICommunication.h"
#include "ClockSyncronization/TimeStamp.h"
#include "ClockSyncronization/TimeController.h"
#include "IData.h"
namespace Communication
{
class CommunicationManager
{
private:
	ICommunication*                      canController;
	ClockSyncronization::TimeController* timeController;
	TimeStamp                            globalTimeStamp;
public:
	CommunicationManager(ClockSyncronization::TimeController* timecontroller, Communication::ICommunication* icomm);
	virtual ~CommunicationManager();

	void initialization();
	void receiveData(void);
	bool selfTest();

};
}
