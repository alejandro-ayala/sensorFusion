#pragma once
#include "TestInclude.h"
class TaskManager
{
private:

public:
	TaskManager();
	virtual ~TaskManager();

	void initialization(int deviceId);
	void send(XCanPs *InstancePtr);
	void receive(XCanPs *InstancePtr);
};
