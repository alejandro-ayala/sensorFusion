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

	TVIRTUAL void initialization();
	TVIRTUAL void syncSharedClock(void);
	TVIRTUAL void selfTest();

};
}
