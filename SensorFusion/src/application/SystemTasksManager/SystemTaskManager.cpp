#include "SystemTasksManager/SystemTasksManager.h"
#include "Logger/LoggerMacros.h"
#include "business_logic/DataSerializer/Image3DSnapshot.h"
#include "business_logic/ImageCapturer3D/LidarPoint.h"
#include "business_logic/Communication/CanMsg.h"
namespace application
{
QueueHandle_t xPointerQueue = NULL;

SystemTasksManager::SystemTasksManager(TaskParams&& systemTaskMngParams) : m_commMng(systemTaskMngParams.commMng)
{
	m_image3DCapturer = std::move(systemTaskMngParams.image3dCapturer);
	m_globalClkMng    = std::move(systemTaskMngParams.globalClkMng);

	//TODO check not NULL pointers
}

void SystemTasksManager::globalClockSyncronization(void* argument)
{
	LOG_INFO("Starting globalClockSyncronization");

	m_globalClkMng->initialization();
	const TickType_t taskSleep = pdMS_TO_TICKS( 30000 );

	while(1)
	{
		LOG_DEBUG("Sending global master time: ", std::to_string(m_globalClkMng->getAbsotuleTime()), " ns") ;
		m_globalClkMng->sendGlobalTime();
		vTaskDelay( taskSleep );
	}
}

void SystemTasksManager::communicationTask(void* argument)
{
	LOG_INFO("SystemTasksManager::Starting communicationTask");
	const TickType_t taskSleep = pdMS_TO_TICKS( 10 );
	business_logic::Communication::CommunicationManager* commMng = reinterpret_cast<business_logic::Communication::CommunicationManager*>(argument);
	commMng->initialization();
  /* Infinite loop */

	while(1)
	{
		commMng->receiveData();
		vTaskDelay( taskSleep );
	}
}
	
void SystemTasksManager::image3dMappingTask(void* argument)
{
	LOG_INFO("SystemTasksManager::Starting image3dMappingTask");

	const TickType_t taskSleep = pdMS_TO_TICKS( 20000 );
  /* Infinite loop */

	while(1)
	{

	}  
}

bool SystemTasksManager::isPendingData()
{
	return (m_capturesQueue->getStoredMsg() > 0);
}

void SystemTasksManager::getNextImage(std::array<business_logic::LidarPoint, business_logic::IMAGE3D_SIZE>& lastCapture)
{
	//m_capturesQueue->receive(&lastCapture);
}

void SystemTasksManager::splitCborToCanMsgs(uint8_t canMsgId, const std::vector<uint8_t>& cborSerializedChunk, std::vector<business_logic::Communication::CanMsg>& canMsgChunks)
{
    size_t totalBytes = cborSerializedChunk.size();
    size_t payloadSize = MAXIMUM_CAN_MSG_SIZE - ID_FIELD_SIZE;
    size_t numberOfMsgs = (totalBytes + payloadSize - 1) / payloadSize;

    for (size_t i = 0; i < numberOfMsgs; ++i)
    {
        business_logic::Communication::CanMsg canMsg;

        canMsg.canMsgId = canMsgId;
        if(i == (numberOfMsgs - 1))
        {
            canMsg.lsbCanMsgIndex = 0xFF;
            canMsg.msbCanMsgIndex = 0xFF;
        }
        else
        {
            canMsg.lsbCanMsgIndex = static_cast<uint8_t>(i & 0xFF);
            canMsg.msbCanMsgIndex = static_cast<uint8_t>((i & 0xFF00) >> 8);
        }


        size_t startIdx = i * payloadSize;
        size_t endIdx = std::min(startIdx + payloadSize, totalBytes);
        canMsg.payloadSize = endIdx - startIdx;

        for (size_t j = startIdx; j < endIdx; ++j)
        {
            canMsg.payload[j - startIdx] = cborSerializedChunk[j];
        }
        canMsgChunks.push_back(canMsg);
    }
}

void SystemTasksManager::createPoolTasks()
{
	LOG_INFO("Creating pool tasks");
	m_clockSyncTaskHandler       = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::globalClockSyncronization, "GlobalClockSyncronization", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(m_globalClkMng.get()), 1024);
	//m_image3dCapturerTaskHandler = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::image3dMappingTask, "image3dMappingTask", DefaultPriorityTask + 1, /*static_cast<business_logic::Osal::VoidPtr>(m_image3DCapturer.get())*/(void*)1, 4096);
	m_commTaskHandler            = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::communicationTask, "CommunicationTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(m_commMng.get()), 1024);
	LOG_INFO("Created pool tasks");
}

}
