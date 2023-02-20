
#include "CommunicationManager.h"
#include "CAN/CanController.h"
#include "CAN/CanIDs.h"
#include "IData.h"
#include <iostream>
using namespace ClockSyncronization;
using namespace Hardware;
namespace Communication
{

CommunicationManager::CommunicationManager(TimeController* timecontroller, CanController* cancontroller)  : timeController(timecontroller), canController(cancontroller)
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

IData CommunicationManager::receiveData()
{
	uint8_t lenght = 800;
	uint8_t data[lenght];
	const auto rxMsg = canController->receiveMsg();
	IData parsedMsg;
	if(rxMsg.dlc > 0)
	{
		//xil_printf("\n\rReceived data: %d bytes", msgSize);

		parsedMsg.deSerialize(data);
		std::cout << "newData[" << parsedMsg.secCounter << "]. sec: " << parsedMsg.timestamp << std::endl;
	}
	return parsedMsg;
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
