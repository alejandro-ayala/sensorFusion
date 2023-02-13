
#include "CommunicationManager.h"
#include "CanController.h"
#include "CanIDs.h"
#include "IData.h"
#include <iostream>
using namespace ClockSyncronization;

namespace Communication
{

CommunicationManager::CommunicationManager(TimeController* timecontroller, Communication::ICommunication* icomm)  : timeController(timecontroller), canController(icomm)
{
}

CommunicationManager::~CommunicationManager()
{
}

void CommunicationManager::initialization()
{
	//canController->initialize();
}

void CommunicationManager::sendData(IData msg)
{
	uint64_t localNs = timeController->getLocalTime();
	msg.timestamp = localNs;
	uint8_t serializedMsg[20];
	uint8_t frameSize = msg.serialize(serializedMsg) + 7;
	std::cout << "sendingMsg[ " << serializedMsg[5] << "]" << msg.timestamp << "-" << serializedMsg[7] << serializedMsg[8] << serializedMsg[9] << serializedMsg[10] << std::endl;

	canController->transmitMsg(static_cast<uint8_t>(CAN_IDs::SENSOR_DATA), serializedMsg,frameSize);

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
