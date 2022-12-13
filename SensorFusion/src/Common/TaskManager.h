#pragma once
#include "TestInclude.h"
class TaskManager
{
private:

public:
	TaskManager();
	virtual ~TaskManager();

	TVIRTUAL void initialization(int deviceId);
	TVIRTUAL void send(XCanPs *InstancePtr);
	TVIRTUAL void receive(XCanPs *InstancePtr);
};
