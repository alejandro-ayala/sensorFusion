#include "SystemTasksManager/SystemTasksManager.h"
#include "Logger/LoggerMacros.h"
#include "business_logic/DataSerializer/Image3DSnapshot.h"
#include "business_logic/ImageCapturer3D/LidarPoint.h"
#include "business_logic/Communication/CanMsg.h"
namespace application
{
QueueHandle_t xPointerQueue = NULL;

SystemTasksManager::SystemTasksManager(TaskParams&& systemTaskMngParams) :  m_globalClkMng(systemTaskMngParams.globalClkMng), m_commMng(systemTaskMngParams.commMng)
{
	m_image3DCapturer = std::move(systemTaskMngParams.image3dCapturer);

	uint32_t queueItemSize   = sizeof(business_logic::LidarArray*); //sizeof(business_logic::Image3DSnapshot*);
	uint32_t queueLength     = 10;
	m_capturesQueue = std::make_shared<business_logic::Osal::QueueHandler>(queueLength, queueItemSize);

	//TODO check not NULL pointers
}

void SystemTasksManager::globalClockSyncronization(void* argument)
{
	business_logic::ClockSyncronization::SharedClockSlaveManager* timeBaseMng = reinterpret_cast<business_logic::ClockSyncronization::SharedClockSlaveManager*>(argument);
	LOG_INFO("Starting globalClockSyncronization");

	timeBaseMng->initialization();
	const TickType_t taskSleep = pdMS_TO_TICKS( 100 );

	while(1)
	{
		LOG_TRACE("Updating global master time");
		const bool isTimeUpdated = timeBaseMng->getGlobalTime();
		if(isTimeUpdated)
		{
			const auto updatedTime = timeBaseMng->getTimeReference().toNs();
			LOG_INFO("Updated global master time: ", updatedTime);
		}
		vTaskDelay( taskSleep );
	}
}

void SystemTasksManager::communicationTask(void* argument)
{
	LOG_INFO("Starting communicationTask");
	const TickType_t taskSleep = pdMS_TO_TICKS( 30000 );

	business_logic::Communication::CommunicationManager* commMng = reinterpret_cast<business_logic::Communication::CommunicationManager*>(argument);
	commMng->initialization();
  /* Infinite loop */

	while(1)
	{
		if(isPendingData())
		{
			LOG_DEBUG("Sending last capture to master node");
			business_logic::LidarArray* lastCapture;

			//getNextImage(lastCapture);
			m_capturesQueue->receive((void*&)lastCapture);
			auto imageSnapshot = std::make_unique<business_logic::Image3DSnapshot>(0x01, 0x00, std::make_shared<business_logic::LidarArray>(*lastCapture), lastCapture->size(), 0x34567811);
			std::vector<uint8_t> serializedImageSnapshot;
			imageSnapshot->serialize(serializedImageSnapshot);
			std::vector<business_logic::Communication::CanMsg> canMsgChunks;
			splitCborToCanMsgs(static_cast<uint8_t>(business_logic::Communication::CAN_IDs::LIDAR_3D_IMAGE), serializedImageSnapshot, canMsgChunks);
			LOG_TRACE("Sending snapshot to external nodes");
			commMng->sendData(canMsgChunks);
			LOG_TRACE("Snapshot sent to external nodes");

		}
		vTaskDelay( taskSleep );
	}
}
	
void SystemTasksManager::image3dMappingTask(void* argument)
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
			m_image3DCapturer->captureImage();

			auto last3dSample = m_image3DCapturer->getLastCapture();

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
    size_t payloadSize = MAXIMUM_CAN_MSG_SIZE - 2;
    size_t numberOfMsgs = (totalBytes + payloadSize - 1) / payloadSize;

    for (size_t i = 0; i < numberOfMsgs; ++i)
    {
        business_logic::Communication::CanMsg canMsg;

        canMsg.canMsgId =canMsgId;
        canMsg.canMsgIndex = static_cast<uint8_t>(i);
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
	m_clockSyncTaskHandler       = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::globalClockSyncronization, "GlobalClockSyncronization", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(m_globalClkMng.get()), 4096);
	m_image3dCapturerTaskHandler = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::image3dMappingTask, "image3dMappingTask", DefaultPriorityTask + 1, /*static_cast<business_logic::Osal::VoidPtr>(m_image3DCapturer.get())*/(void*)1, 4096);
	m_commTaskHandler            = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::communicationTask, "CommunicationTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(m_commMng.get()), 4096);
	LOG_INFO("Created pool tasks");
}

}
