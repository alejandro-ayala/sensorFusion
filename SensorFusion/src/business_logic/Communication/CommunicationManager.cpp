
#include <business_logic/Communication/CanIDs.h>
#include <business_logic/Communication/CommunicationManager.h>
#include <business_logic/Communication/IData.h>
#include "hardware_abstraction/Controllers/CAN/CanController.h"
#include "services/Logger/LoggerMacros.h"
#include <iostream>

static inline TaskHandle_t xReceiveTaskToNotify;
namespace business_logic
{
using namespace ClockSyncronization;
using namespace hardware_abstraction::Controllers;
namespace Communication
{
#ifdef ASSEMBLER_TASK
CommunicationManager::CommunicationManager(const std::shared_ptr<ClockSyncronization::TimeController>& timecontroller, const std::shared_ptr<hardware_abstraction::Controllers::PsCanController>& cancontroller, const std::shared_ptr<business_logic::Osal::QueueHandler>& cameraFramesQueue)  : timeController(timecontroller), canController(cancontroller)
{
	xReceiveTaskToNotify = xTaskGetCurrentTaskHandle();
	msgGateway = std::make_shared<MsgGateway>(cameraFramesQueue);
#else
CommunicationManager::CommunicationManager(const std::shared_ptr<ClockSyncronization::TimeController>& timecontroller, const std::shared_ptr<hardware_abstraction::Controllers::PsCanController>& cancontroller, const std::shared_ptr<business_logic::Osal::QueueHandler>& cameraFramesQueue, const std::shared_ptr<business_logic::ImageAssembler::ImageAssembler>& imageAssembler)  : timeController(timecontroller), canController(cancontroller)
{
	msgGateway = std::make_shared<MsgGateway>(cameraFramesQueue, imageAssembler);
#endif
}

CommunicationManager::~CommunicationManager()
{
}

void CommunicationManager::initialization(const TaskHandle_t& taskToNotify)
{
#ifdef ASSEMBLER_TASK
	msgGateway->initialization(taskToNotify);
#endif
	canController->initialize();
	const auto xTaskToNotify = xTaskGetCurrentTaskHandle();
	canController->registerTaskToNotify(xTaskToNotify);
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
        //for (int Delay = 0; Delay < 0xFFFF; Delay++);
    }
    return result;
}

bool CommunicationManager::receiveData()
{
	bool assembleCbor = false;
	bool isEndOfImage = false;
	auto rxMsgVector = canController->receiveMsg(assembleCbor, isEndOfImage);

	static uint8_t lastFrameIndex = 0;
	static uint8_t lastCborIndex  = 0;

	uint8_t msgId = 0;

	if(assembleCbor)
	{
		LOG_TRACE("CommunicationManager::receiveData from canController: ", std::to_string(rxMsgVector.size()));
	//TODO replace the loop and request only the last sample to get the variables IDs and Idx
		for(auto& rxMsg : rxMsgVector)
		{

			if(rxMsg.dlc > 0)
			{
				std::string frameSize = "Received frame of size" + std::to_string(rxMsg.dlc) + " : ";
				LOG_TRACE(frameSize, std::to_string(rxMsg.data[0]), " ", std::to_string(rxMsg.data[1]), " " , std::to_string(rxMsg.data[2]), " ", std::to_string(rxMsg.data[3]), " ", std::to_string(rxMsg.data[4]), " ", std::to_string(rxMsg.data[5]), " ", std::to_string(rxMsg.data[6]), " ", std::to_string(rxMsg.data[7]));
				if(rxMsg.dlc != 8)
				{
					//Completamos con 0xFF los bytes pendientes del final de trama
					for (size_t i = rxMsg.dlc; i < CAN_DATA_PAYLOAD_SIZE; ++i)
					{
						rxMsg.data[i] = 0xFF;
					}
				}

				msgGateway->storeMsg(rxMsg.id, rxMsg.data);
				lastFrameIndex = rxMsg.data[0];
				lastCborIndex  = rxMsg.data[1];
				msgId = rxMsg.id;
			}
		}
	}

	if(assembleCbor)
	{
		LOG_TRACE("CommunicationManager::receiveData SENT FRAME_CONFIRMATION for ", std::to_string(((lastFrameIndex & 0xC0) >> 6)), " -- ", std::to_string((lastFrameIndex & 0x3F)), " -- ", std::to_string(isEndOfImage));
		msgGateway->completedFrame(msgId, lastFrameIndex, lastCborIndex, isEndOfImage);
		LOG_TRACE("CommunicationManager::receiveData COMPLETED FRAME done");
		canController->clearBuffer();
		//Sending confirmation to complete assembled frame
		uint8_t data[MAXIMUM_CAN_MSG_SIZE] = {0x1,0x1,0x2,0x2,0x3,0x3,0x4,0x4};
		LOG_TRACE("CommunicationManager::receiveData Sending FRAME_CONFIRMATION for ", std::to_string(((lastFrameIndex & 0xC0) >> 6)), " -- ", std::to_string((lastFrameIndex & 0x3F)));
		canController->transmitMsg(static_cast<uint8_t>(CAN_IDs::FRAME_CONFIRMATION), data, MAXIMUM_CAN_MSG_SIZE);
	}

	return assembleCbor;
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
