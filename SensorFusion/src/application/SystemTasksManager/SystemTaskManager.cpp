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
	LOG_INFO("Starting globalClockSyncronization");

	m_globalClkMng->initialization();
	const TickType_t taskSleep = pdMS_TO_TICKS( 100 );

	while(1)
	{
		LOG_TRACE("Updating global master time");
		const bool isTimeUpdated = m_globalClkMng->getGlobalTime();
		if(isTimeUpdated)
		{
			const auto updatedTime = m_globalClkMng->getTimeReference().toNs();
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
		static uint8_t msgId = 0;
		if(isPendingData())
		{

			business_logic::LidarArray* lastCapture;

			//getNextImage(lastCapture);
			m_capturesQueue->receive((void*&)lastCapture);
			auto imageSnapshot = std::make_unique<business_logic::Image3DSnapshot>(msgId, 0x00, std::make_shared<business_logic::LidarArray>(*lastCapture), lastCapture->size(), m_lastCaptureTimestampStart, m_lastCaptureTimestampEnd);
			LOG_DEBUG("3D image ID: " , std::to_string(msgId), " -- size: ", std::to_string(lastCapture->size()));
			std::vector<uint8_t> serializedImageSnapshot;
			imageSnapshot->serialize(serializedImageSnapshot);
			std::vector<business_logic::Communication::CanMsg> canMsgChunks;
			splitCborToCanMsgs(msgId, serializedImageSnapshot, canMsgChunks);
			LOG_DEBUG("Sending last capture to master node with: ", std::to_string(canMsgChunks.size()), " CAN messages");
			commMng->sendData(canMsgChunks);
			LOG_TRACE("Snapshot sent to external nodes");
			msgId++;

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
			m_lastCaptureTimestampStart = m_globalClkMng->getLocalTime();
			m_image3DCapturer->captureImage();

			auto last3dSample = m_image3DCapturer->getLastCapture();
			m_lastCaptureTimestampEnd = m_globalClkMng->getLocalTime();
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
	m_clockSyncTaskHandler       = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::globalClockSyncronization, "GlobalClockSyncronization", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(m_globalClkMng.get()), 4096);
	m_image3dCapturerTaskHandler = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::image3dMappingTask, "image3dMappingTask", DefaultPriorityTask + 1, /*static_cast<business_logic::Osal::VoidPtr>(m_image3DCapturer.get())*/(void*)1, 4096);
	m_commTaskHandler            = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::communicationTask, "CommunicationTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(m_commMng.get()), 4096);
	LOG_INFO("Created pool tasks");
}

}
