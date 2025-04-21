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
	uint32_t queueItemSize   = sizeof(business_logic::LidarArray*); //sizeof(business_logic::Image3DSnapshot*);
	uint32_t queueLength     = 10;
	m_capturesQueue = std::make_shared<business_logic::Osal::QueueHandler>(queueLength, queueItemSize);
	//TODO check not NULL pointers
}

void SystemTasksManager::globalClockSyncronization(void* argument)
{
	m_globalClkMng->initialization();
	const TickType_t taskSleep = pdMS_TO_TICKS( 30000 );
    size_t freeHeapSize = xPortGetFreeHeapSize();
    size_t minEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();
    const std::string startMsg = "SystemTasks::globalClockSyncronization started --> freeHeapSize: " + std::to_string(freeHeapSize) + " minEverFreeHeapSize " + std::to_string(minEverFreeHeapSize);
    LOG_INFO(startMsg);

	while(1)
	{
		LOG_DEBUG("Sending global master time: ", std::to_string(m_globalClkMng->getAbsotuleTime()), " ns") ;
	    size_t freeHeapSize = xPortGetFreeHeapSize();
	    size_t minEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();
	    const std::string freeHeapMsg = "freeHeapSize: " + std::to_string(freeHeapSize) + " minEverFreeHeapSize " + std::to_string(minEverFreeHeapSize);
	    LOG_INFO(freeHeapMsg);
		m_globalClkMng->sendGlobalTime();
		vTaskDelay( taskSleep );
	}
}

void SystemTasksManager::communicationTask(void* argument)
{
	const TickType_t taskSleep = pdMS_TO_TICKS( 1 );
	business_logic::Communication::CommunicationManager* commMng = reinterpret_cast<business_logic::Communication::CommunicationManager*>(argument);
	commMng->initialization();
    size_t freeHeapSize = xPortGetFreeHeapSize();
    size_t minEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();
    const std::string startMsg = "SystemTasks::communicationTask started --> freeHeapSize: " + std::to_string(freeHeapSize) + " minEverFreeHeapSize " + std::to_string(minEverFreeHeapSize);
    LOG_INFO(startMsg);
  /* Infinite loop */

	while(1)
	{
		static uint32_t loopIndex = 0;
//		if(loopIndex > 5000000)
//		{
//			size_t freeHeapSize = xPortGetFreeHeapSize();
//			size_t minEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();
//			const std::string freeHeapMsg = "SystemTasks::communicationTask freeHeapSize: " + std::to_string(freeHeapSize) + " minEverFreeHeapSize " + std::to_string(minEverFreeHeapSize);
//			LOG_INFO(freeHeapMsg);
//			loopIndex = 0;
//		}
		commMng->receiveData();
		loopIndex++;
		vTaskDelay( taskSleep );
	}
}
	
void SystemTasksManager::image3dCapturerTask(void* argument)
{
	LOG_INFO("Starting image3dCapturerTask");
	//business_logic::ImageCapturer3D* image3dCapturer = reinterpret_cast<business_logic::ImageCapturer3D*>(argument);

	const TickType_t taskSleep = pdMS_TO_TICKS( 20000 );
	m_image3DCapturer->initialize();
  /* Infinite loop */

	while(1)
	{
		static uint8_t captureId = 0;
		try
		{
			LOG_DEBUG("Capturing 3D image");
			m_lastCaptureTimestampStart = m_globalClkMng->getAbsotuleTime();
			m_image3DCapturer->captureImage();

			auto last3dSample = m_image3DCapturer->getLastCapture();
			m_lastCaptureTimestampEnd = m_globalClkMng->getAbsotuleTime();
			const auto captureDeltaTime = m_lastCaptureTimestampEnd - m_lastCaptureTimestampStart;
			LOG_DEBUG("Start capture at ", std::to_string(m_lastCaptureTimestampStart));
			LOG_DEBUG("End capture at ", std::to_string(m_lastCaptureTimestampEnd));

			LOG_DEBUG("3D image captured in ", std::to_string(captureDeltaTime), " ns");
			business_logic::LidarArray* pxPointerToxMessage;
			pxPointerToxMessage = &last3dSample;
			m_capturesQueue->sendToBack(( void * ) &pxPointerToxMessage);
			captureId++;
			LOG_TRACE("Capturing 3D image done");

			vTaskDelay( taskSleep );
		}
		catch (const services::BaseException& e)
		{
			LOG_ERROR("Exception [", e.getErrorId() ,"]while captureImage: ", e.what());
		}
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
	//m_clockSyncTaskHandler       = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::globalClockSyncronization, "GlobalClockSyncronization", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(m_globalClkMng.get()), 1024);
	m_image3dCapturerTaskHandler = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::image3dCapturerTask, "image3dCapturerTask", DefaultPriorityTask + 1, /*static_cast<business_logic::Osal::VoidPtr>(m_image3DCapturer.get())*/(void*)1, 4096);
	//m_commTaskHandler            = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::communicationTask, "CommunicationTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(m_commMng.get()), 2048);
	LOG_INFO("Created pool tasks");
}

}
