
#include "CommunicationManager.h"
#include "CanController.h"
#include <iostream>

namespace Communication
{
CommunicationManager::CommunicationManager(ClockSyncronization::TimeController* timecontroller, Communication::ICommunication* icomm): timeController(timecontroller) , canController(icomm)
{

}

CommunicationManager::~CommunicationManager()
{

}

void CommunicationManager::initialization()
{
	//canController->initialize();
}

void CommunicationManager::receiveData()
{
	uint8_t lenght = 800;
	uint8_t data[lenght];
	int msgSize = canController->receiveMsg(data);

	if(msgSize > 0)
	{
		//xil_printf("\n\rReceived data: %d bytes", msgSize);
		IData parsedMsg;
		parsedMsg.deSerialize(data);
		std::cout << "newData[" << parsedMsg.secCounter << "]. sec: " << parsedMsg.timestamp << std::endl;
	}
}

bool CommunicationManager::selfTest()
{
	initialization();
	xil_printf("selfTest\r\n");
	bool result ;//= canController->selfTest();
	if(result)	xil_printf("PASS\r\n");
	else xil_printf("FAILED\r\n");
	return result;
}
}
