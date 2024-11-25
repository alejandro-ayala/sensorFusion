
#include <business_logic/Communication/CanIDs.h>
#include <business_logic/Communication/CommunicationManager.h>
#include <business_logic/Communication/IData.h>
#include "hardware_abstraction/Controllers/CAN/CanController.h"
#include "services/Logger/LoggerMacros.h"
#include <iostream>


namespace business_logic
{
using namespace ClockSyncronization;
using namespace hardware_abstraction::Controllers;
namespace Communication
{
CommunicationManager::CommunicationManager(const std::shared_ptr<ClockSyncronization::TimeController>& timecontroller, const std::shared_ptr<hardware_abstraction::Controllers::CanController>& cancontroller)  : timeController(timecontroller), canController(cancontroller)
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
	LOG_DEBUG("sendingMsg[ " , serializedMsg[5] , "]" , msg.timestamp , "-" , serializedMsg[7] , serializedMsg[8] , serializedMsg[9] , serializedMsg[10]);

	canController->transmitMsg(static_cast<uint8_t>(CAN_IDs::SENSOR_DATA), serializedMsg,frameSize);

}

void CommunicationManager::sendData(const std::vector<business_logic::Communication::CanMsg>& dataToSend)
{
    const uint8_t msgToSend = dataToSend.size();

    for(int currentMsgIndex = 0; currentMsgIndex < msgToSend; currentMsgIndex++)
    {
    	const auto& frame = dataToSend.at(currentMsgIndex);
        uint8_t data[MAXIMUM_CAN_MSG_SIZE] = {0};
        data[0] = frame.canMsgId;
        data[1] = frame.canMsgIndex;
        uint8_t dataSize = ID_FIELD_SIZE + frame.payloadSize;
        for (uint8_t i = ID_FIELD_SIZE; i < MAXIMUM_CAN_MSG_SIZE; i++)
        {
            data[i] = frame.payload[i - ID_FIELD_SIZE];
        }

        //canController->transmitMsg(static_cast<uint8_t>(CAN_IDs::LIDAR_3D_IMAGE), data, dataSize);
        //TODO check and remove the delay
        const uint8_t delayBetweenFrames = 2;
        LOG_DEBUG("Send CAN msgId: ", frame.canMsgId, " msgIndex: " , frame.canMsgIndex);
        for (int Delay = 0; Delay < 0xFFFF; Delay++);
    }
}

IData CommunicationManager::receiveData()
{
	uint8_t lenght = 800;
	uint8_t data[lenght];
	const auto rxMsg = canController->receiveMsg();
	IData parsedMsg;
	if(rxMsg.dlc > 0)
	{
		parsedMsg.deSerialize(data);
		LOG_DEBUG("newData[" , parsedMsg.secCounter , "]. sec: " , parsedMsg.timestamp);
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
}
