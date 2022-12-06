
#include "CommunicationManager.h"

namespace Communication
{
CommunicationManager::CommunicationManager()
{
	canController = new CanController();
}

CommunicationManager::~CommunicationManager()
{
	delete canController;
}

void CommunicationManager::initialization()
{
	canController->initialize();
}

void CommunicationManager::syncSharedClock()
{
	xil_printf("syncSharedClock\r\n");
	canController->sendFrame();
}

void CommunicationManager::selfTest()
{
	initialization();
	xil_printf("selfTest\r\n");
	auto result = canController->selfTest();
	if(result)	xil_printf("PASS\r\n");
	else xil_printf("FAILED\r\n");

}
}
