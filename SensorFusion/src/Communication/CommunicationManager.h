#pragma once
#include "CanController.h"

namespace Communication
{
class CommunicationManager
{
private:
	CanController* canController;
public:
	CommunicationManager();
	virtual ~CommunicationManager();

	void initialization();
	void syncSharedClock(void);
};
}
