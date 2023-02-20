#pragma once
#include "CAN/CanController.h"
#include "IData.h"
#include "ClockSyncronization/TimeStamp.h"
#include "ClockSyncronization/TimeController.h"
namespace Communication
{
class CommunicationManager
{
private:
	Hardware::CanController*             canController;
	ClockSyncronization::TimeController* timeController;
	TimeStamp                            globalTimeStamp;
public:
	CommunicationManager(ClockSyncronization::TimeController* timecontroller, Hardware::CanController* cancontroller);
	virtual ~CommunicationManager();

	void initialization();
	void sendData(IData msg);
	IData receiveData(void);
	bool selfTest();

};
}
