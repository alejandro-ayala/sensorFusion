#include "SystemTasksManager/SystemTasksManager.h"
#include "Logger/LoggerMacros.h"
#include "business_logic/DataSerializer/Image3DSnapshot.h"
#include "business_logic/ImageCapturer3D/LidarPoint.h"
namespace application
{
//#define USE_ABSTRACTION
QueueHandle_t xPointerQueue = NULL;

SystemTasksManager::SystemTasksManager(TaskParams&& systemTaskMngParams) :  m_globalClkMng(systemTaskMngParams.globalClkMng), m_commMng(systemTaskMngParams.commMng)
{
	m_image3DCapturer = std::move(systemTaskMngParams.image3dCapturer);

	uint32_t queueItemSize   = sizeof(business_logic::LidarArray*); //sizeof(business_logic::Image3DSnapshot*);
	uint32_t queueLength     = 10;
#ifdef USE_ABSTRACTION
	m_capturesQueue = std::make_shared<business_logic::Osal::QueueHandler>(queueLength, queueItemSize);
#else
	xPointerQueue = xQueueCreate(queueLength, queueItemSize );
#endif
	//TODO check not NULL pointers
}

void SystemTasksManager::globalClockSyncronization(void* argument)
{
	business_logic::ClockSyncronization::SharedClockSlaveManager* timeBaseMng = reinterpret_cast<business_logic::ClockSyncronization::SharedClockSlaveManager*>(argument);
	LOG_INFO("Starting globalClockSyncronization");

	//timeBaseMng->initialization();
	const TickType_t taskSleep = pdMS_TO_TICKS( 5000 );

	while(1)
	{
		LOG_DEBUG("Updating global master time");
		//timeBaseMng->sendGlobalTime();
		for(int i=0;i<0xFFFFF;i++);
		LOG_DEBUG("Updated global master time");
		vTaskDelay( taskSleep );
	}
}

void SystemTasksManager::communicationTask(void* argument)
{
	LOG_INFO("Starting communicationTask");
	const TickType_t taskSleep = pdMS_TO_TICKS( 1000 );

	//business_logic::Communication::CommunicationManager* commMng = reinterpret_cast<business_logic::Communication::CommunicationManager*>(argument);

  /* Infinite loop */

	while(1)
	{
		if(isPendingData())
		{
			LOG_DEBUG("Sending last capture to master node");
			business_logic::LidarArray* lastCapture;
#ifdef USE_ABSTRACTION

			//getNextImage(lastCapture);
			m_capturesQueue->receive(&lastCapture);
#else
			LOG_DEBUG("Receiving from queue into buffer: %p", lastCapture);
			xQueueReceive( xPointerQueue, &( lastCapture ), ( TickType_t ) 10 );
			LOG_DEBUG("Received pointer from queue: %p", lastCapture);
#endif

			//commMng->receiveData();
			for(int i=0;i<0xFFFFF;i++);
			LOG_DEBUG("Received data from external nodes");

		}
		vTaskDelay( taskSleep );
	}
}
	
void SystemTasksManager::image3dMappingTask(void* argument)
{
	LOG_INFO("Starting image3dCapturerTask");
	//business_logic::ImageCapturer3D* image3dCapturer = reinterpret_cast<business_logic::ImageCapturer3D*>(argument);

	const TickType_t taskSleep = pdMS_TO_TICKS( 2000 );
	m_image3DCapturer->initialize();
  /* Infinite loop */

	while(1)
	{
		static uint8_t captureId = 0;
		try
		{
			LOG_DEBUG("Capturing 3D image");
			m_image3DCapturer->captureImage();

			//auto last3dSample = std::make_shared<std::array<business_logic::LidarPoint, IMAGE3D_SIZE>>(m_image3DCapturer->getLastCapture());
			auto last3dSample = m_image3DCapturer->getLastCapture();

			//LOG_DEBUG("Address of m_3DImage: %p", &last3dSample);

#ifdef USE_ABSTRACTION
			m_capturesQueue->sendToBack(( void * ) &last3dSample);
#else
			business_logic::LidarArray* pxPointerToxMessage;
			pxPointerToxMessage = &last3dSample;
			LOG_DEBUG("Sending pointer to queue: %p", pxPointerToxMessage);
		    xQueueSend(xPointerQueue, ( void * ) &pxPointerToxMessage,  ( TickType_t ) 0 );
#endif

			//LOG_DEBUG("Sending pointer to queue: %p", last3dSample);
			captureId++;
			LOG_DEBUG("Capturing 3D image done");

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

void SystemTasksManager::createPoolTasks()
{
	LOG_INFO("Creating pool tasks");
	m_clockSyncTaskHandler       = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::globalClockSyncronization, "GlobalClockSyncronization", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(m_globalClkMng.get()));
	m_image3dCapturerTaskHandler = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::image3dMappingTask, "image3dMappingTask", DefaultPriorityTask + 1, /*static_cast<business_logic::Osal::VoidPtr>(m_image3DCapturer.get())*/(void*)1, 4096);
	m_commTaskHandler            = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::communicationTask, "CommunicationTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(m_commMng.get()), 4096);
	LOG_INFO("Created pool tasks");
}

}
