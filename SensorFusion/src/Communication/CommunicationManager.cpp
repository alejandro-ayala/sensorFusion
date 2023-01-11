
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
	//canController->sendFrame();
}

}
