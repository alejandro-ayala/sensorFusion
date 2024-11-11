#include "SystemTasksManager/SystemTasksManager.h"
#include "Logger/LoggerMacros.h"

namespace application
{
SystemTasksManager::SystemTasksManager(const TaskParams& systemTaskMngParams) : m_image3DCapturer(systemTaskMngParams.image3dCapturer),	m_globalClkMng(systemTaskMngParams.globalClkMng), m_commMng(systemTaskMngParams.commMng)
{
	//TODO check not NULL pointers
}

void SystemTasksManager::globalClockSyncronization(void* argument)
{
	business_logic::ClockSyncronization::SharedClockSlaveManager* timeBaseMng = reinterpret_cast<business_logic::ClockSyncronization::SharedClockSlaveManager*>(argument);
	LOG_INFO("Starting globalClockSyncronization");

	timeBaseMng->initialization();
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	uint16_t taskSleep = 1000;

	while(1)
	{
		LOG_INFO("Updating global master time");
		//timeBaseMng->sendGlobalTime();
		LOG_INFO("Updated global master time");
		vTaskDelayUntil(&xLastWakeTime,taskSleep / portTICK_RATE_MS);
	}
}

void SystemTasksManager::communicationTask(void* argument)
{
	LOG_INFO("Starting communicationTask");
	business_logic::Communication::CommunicationManager* commMng = reinterpret_cast<business_logic::Communication::CommunicationManager*>(argument);
	TickType_t xLastWakeTime;

	xLastWakeTime = xTaskGetTickCount();
	uint16_t taskSleep = 5000;

  /* Infinite loop */
	while(1)
	{
		LOG_INFO("Receiving data from external nodes");
		//commMng->receiveData();
		LOG_INFO("Received data from external nodes");
		vTaskDelayUntil(&xLastWakeTime,taskSleep / portTICK_RATE_MS);
	}
}
	
void SystemTasksManager::image3dMappingTask(void* argument)
{
	LOG_INFO("Starting image3dCapturerTask");
	business_logic::ImageCapturer3D* image3dCapturer = reinterpret_cast<business_logic::ImageCapturer3D*>(argument);

	TickType_t xLastWakeTime;
    image3dCapturer->initialize();
	xLastWakeTime = xTaskGetTickCount();
	uint16_t taskSleep = 20000;


  /* Infinite loop */
	while(1)
	{
		LOG_INFO("Capturing 3D image");
		image3dCapturer->captureImage();
		LOG_INFO("Capturing 3D image done");
		//vTaskDelayUntil(xLastWakeTime,5000 / portTICK_RATE_MS);
		vTaskDelayUntil(&xLastWakeTime,taskSleep / portTICK_RATE_MS);
		//vTaskDelay(5 / portTICK_RATE_MS);
	}  
}

void SystemTasksManager::createPoolTasks()
{
	LOG_INFO("Creating pool tasks");
	m_clockSyncTaskHandler       = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::globalClockSyncronization, "GlobalClockSyncronization", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(m_globalClkMng.get()));
	m_image3dCapturerTaskHandler = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::image3dMappingTask, "image3dMappingTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(m_image3DCapturer.get()), 4096);
	m_commTaskHandler            = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::communicationTask, "CommunicationTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(m_commMng.get()), 4096);
	LOG_INFO("Created pool tasks");
}

}
