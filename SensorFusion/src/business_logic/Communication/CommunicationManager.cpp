
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
	canController->initialize();
}

bool CommunicationManager::sendData(IData msg)
{
	uint64_t localNs = timeController->getLocalTime();
	msg.timestamp = localNs;
	uint8_t serializedMsg[20];
	uint8_t frameSize = msg.serialize(serializedMsg) + 7;
	//LOG_DEBUG("sendingMsg[ " , serializedMsg[5] , "]" , msg.timestamp , "-" , serializedMsg[7] , serializedMsg[8] , serializedMsg[9] , serializedMsg[10]);
	LOG_INFO("Sending msg");
	const auto result = canController->transmitMsg(static_cast<uint8_t>(CAN_IDs::SENSOR_DATA), serializedMsg,frameSize);
	return result;
}

bool CommunicationManager::sendData(const std::vector<business_logic::Communication::CanMsg>& dataToSend)
{
    const size_t msgToSend = dataToSend.size();
    bool result = true;
    for(size_t currentMsgIndex = 0; currentMsgIndex < msgToSend; currentMsgIndex++)
    {
    	const auto& frame = dataToSend.at(currentMsgIndex);
        uint8_t data[MAXIMUM_CAN_MSG_SIZE] = {0};
        data[0] = frame.canMsgId;
        data[1] = frame.lsbCanMsgIndex;
        data[2] = frame.msbCanMsgIndex;
        uint8_t dataSize = ID_FIELD_SIZE + frame.payloadSize;
        for (uint8_t i = ID_FIELD_SIZE; i < MAXIMUM_CAN_MSG_SIZE; i++)
        {
            data[i] = frame.payload[i - ID_FIELD_SIZE];
        }

        result &= canController->transmitMsg(static_cast<uint8_t>(CAN_IDs::LIDAR_3D_IMAGE), data, dataSize);
        //TODO check and remove the delay
        const size_t msgIndex = ((frame.msbCanMsgIndex << 8) | frame.lsbCanMsgIndex);
        if(!result)
        	LOG_ERROR("Error sending CAN msgId: ", frame.canMsgId, " msgIndex: " , std::to_string(msgIndex), " -- currentMsgIndex: ", std::to_string(currentMsgIndex));
        else
        	LOG_TRACE("Send CAN msgId: ", frame.canMsgId, " msgIndex: " , std::to_string(msgIndex), " -- currentMsgIndex: ", std::to_string(currentMsgIndex));
        for (int Delay = 0; Delay < 0xFFFF; Delay++);
    }
    return result;
}

IData CommunicationManager::receiveData()
{
	uint8_t lenght = 800;
	uint8_t data[lenght];
	const auto rxMsg = canController->receiveMsg();
	IData parsedMsg;
	if(rxMsg.dlc > 0)
	{
		LOG_INFO(std::to_string(rxMsg.id), ",", std::to_string(rxMsg.data[0]), ",", std::to_string(rxMsg.data[1]));
		//parsedMsg.deSerialize(data);
		//LOG_DEBUG("newData[" , parsedMsg.secCounter , "]. sec: " , parsedMsg.timestamp);
	}
	return parsedMsg;
}

bool CommunicationManager::selfTest()
{
//	initialization();
//	xil_printf("selfTest\r\n");
//	bool result ;//= canController->selfTest();
//	if(result)	xil_printf("PASS\r\n");
//	else xil_printf("FAILED\r\n");
	return true;
}
}
}
