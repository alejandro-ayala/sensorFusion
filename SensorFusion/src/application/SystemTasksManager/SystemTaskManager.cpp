#include "SystemTasksManager/SystemTasksManager.h"
#include "Logger/LoggerMacros.h"
#include "business_logic/DataSerializer/Image3DSnapshot.h"
namespace application
{
SystemTasksManager::SystemTasksManager(TaskParams&& systemTaskMngParams) :  m_globalClkMng(systemTaskMngParams.globalClkMng), m_commMng(systemTaskMngParams.commMng)
{
	m_image3DCapturer = std::move(systemTaskMngParams.image3dCapturer);

	uint32_t queueItemSize   = sizeof(business_logic::Image3DSnapshot);
	uint32_t queueLength     = 10;
	m_capturesQueue = std::make_shared<business_logic::Osal::QueueHandler>(queueLength, queueItemSize);

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
			business_logic::Image3DSnapshot lastCapture;
			getNextImage(lastCapture);
			//commMng->receiveData();
			for(int i=0;i<0xFFFFF;i++);
			LOG_DEBUG("Received data from external nodes");
			vTaskDelay( taskSleep );
		}

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
		try
		{
			LOG_DEBUG("Capturing 3D image");
			m_image3DCapturer->captureImage();

			auto lastCapture = m_image3DCapturer->getLastCapture();
			m_capturesQueue->sendToBack(( void * ) &lastCapture);

			LOG_DEBUG("Capturing 3D image done");
			vTaskDelay( taskSleep );
		}
		catch (std::exception e)
		{
			LOG_ERROR("Exception while captureImage: ", e.what());
		}
	}  
}

bool SystemTasksManager::isPendingData()
{
	return (m_capturesQueue->getStoredMsg() > 0);
}

void SystemTasksManager::getNextImage(business_logic::Image3DSnapshot& lastCapture)
{
	m_capturesQueue->receive(&lastCapture);
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
