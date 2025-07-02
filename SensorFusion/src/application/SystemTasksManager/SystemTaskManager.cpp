#include "SystemTasksManager/SystemTasksManager.h"
#include "Logger/LoggerMacros.h"
#include "business_logic/DataSerializer/Image3DSnapshot.h"
#include "business_logic/ImageCapturer3D/LidarPoint.h"
#include "business_logic/Communication/CanMsg.h"
namespace application
{

extern "C" {
#include "xil_io.h"
#include "xtime_l.h"

#define MAX_REGISTRIES 10
typedef struct {
    const char *taskName;
    uint32_t totalTime;
    uint32_t minTime;
    uint32_t maxTime;
    uint32_t totalExecutions;
} RunTimeStats_t;

volatile bool sentGlobalTime = false;
static RunTimeStats_t runtimeStatsRegistries[MAX_REGISTRIES];
static uint8_t currentRegisteredStats = 0;



#include <string.h>

extern uint32_t ulGetRunTimeCounterValue(void);

void RunTimeStats_Start(uint32_t *timestamp)
{
    *timestamp = ulGetRunTimeCounterValue();
}

void RunTimeStats_End(const char *taskName, uint32_t timestamp_start, bool printSample = false)
{
    uint32_t duration = ulGetRunTimeCounterValue() - timestamp_start;
//#ifdef GENERATE_RUNTIME_STATS
    RunTimeStats_t *currentTime = NULL;
    for (int i = 0; i < currentRegisteredStats; i++) {
        if (strcmp(runtimeStatsRegistries[i].taskName, taskName) == 0) {
        	currentTime = &runtimeStatsRegistries[i];
            break;
        }
    }

    // Create new instance
    if (currentTime == NULL && currentRegisteredStats < MAX_REGISTRIES) {
    	runtimeStatsRegistries[currentRegisteredStats].taskName = taskName;
    	runtimeStatsRegistries[currentRegisteredStats].totalTime = 0;
    	runtimeStatsRegistries[currentRegisteredStats].minTime = 0xFFFFFFFF;
    	runtimeStatsRegistries[currentRegisteredStats].maxTime = 0;
    	runtimeStatsRegistries[currentRegisteredStats].totalExecutions = 0;
        currentTime = &runtimeStatsRegistries[currentRegisteredStats++];
    }

    if (currentTime) {
        currentTime->totalTime += duration;
        if (duration < currentTime->minTime) currentTime->minTime = duration;
        if (duration > currentTime->maxTime) currentTime->maxTime = duration;
        currentTime->totalExecutions++;
    }
//#else
    if(printSample)
    {
        const std::string strRuntime = std::string(taskName) + " executed in " + std::to_string(duration) + " us";
        LOG_DEBUG(strRuntime);
    }
//#endif
}
#include <sstream>
#include <string>

std::string RunTimeStats_FormatLog(const RunTimeStats_t& r)
{
    std::ostringstream oss;
    uint32_t averageTime = (r.totalExecutions > 0) ? (r.totalTime / r.totalExecutions) : 0;

    oss << r.taskName << ": executions=" << r.totalExecutions
        << ", min=" << r.minTime << " us"
        << ", max=" << r.maxTime << " us"
        << ", prom=" << averageTime << " us";

    return oss.str();
}


void RunTimeStats_Print(void)
{

    LOG_INFO("Runtime execution time");
    for (int i = 0; i < currentRegisteredStats; i++) {
    	std::string logEntry = RunTimeStats_FormatLog(runtimeStatsRegistries[i]);
    	LOG_INFO(logEntry);
    }
}

void vConfigureTimerForRunTimeStats(void)
{
    //Nothing TODO, it is already provided by XTime_GetTime
}


uint32_t ulGetRunTimeCounterValue(void)
{
    XTime ticks;
    XTime_GetTime(&ticks);
    return static_cast<uint32_t>(ticks / (XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ / 2 / 1000000));
}

}

#ifdef ASSEMBLER_TASK
TaskHandle_t taskHandlerImgAssembler;
#endif
TaskHandle_t taskHandlerCommunication;
TaskHandle_t taskHandlerSensorFusion;
TaskHandle_t taskHandlerImgClassifier;

QueueHandle_t xPointerQueue = NULL;

SystemTasksManager::SystemTasksManager(TaskParams&& systemTaskMngParams) : m_commMng(systemTaskMngParams.commMng)
{
	m_image3DCapturer = std::move(systemTaskMngParams.image3dCapturer);
	m_globalClkMng    = std::move(systemTaskMngParams.globalClkMng);
	uint32_t queueItemSize   = sizeof(business_logic::LidarArray*); //sizeof(business_logic::Image3DSnapshot*);
	uint32_t queueLength     = 10;
	m_capturesQueue = std::make_shared<business_logic::Osal::QueueHandler>(queueLength, queueItemSize);

	m_imageClassifier =  std::make_shared<business_logic::ImageClassifier::ImageClassifierManager>(systemTaskMngParams.imageProvider);
	m_imageAssembler    = std::move(systemTaskMngParams.imageAssembler);

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

    uint32_t startExecutionTime;
    uint32_t triggerSystemStats = 0;

	while(1)
	{
		RunTimeStats_Start(&startExecutionTime);
		const auto t1 = xTaskGetTickCount();
		LOG_DEBUG("Sending global master time: ", std::to_string(m_globalClkMng->getAbsotuleTime()), " ns") ;
		logMemoryUsage();
		if(!sentGlobalTime)
		{
			m_globalClkMng->sendGlobalTime();
			sentGlobalTime = true;
		}
		const auto executionTime = (xTaskGetTickCount() - t1) * portTICK_PERIOD_MS;
		LOG_DEBUG("SystemTasks::globalClockSyncronization executed in: ", executionTime, " ms");
		RunTimeStats_End("globalClockSyncronization", startExecutionTime, true);

		vTaskDelay( taskSleep );

		if(triggerSystemStats > 10)
		{
			//printSystemStats();
			LOG_INFO("*********System Custom stats*********");
			RunTimeStats_Print();
			LOG_INFO("***************************");
			triggerSystemStats = 0;
		}
		triggerSystemStats++;
	}
}

void SystemTasksManager::communicationTask(void* argument)
{

	const TickType_t taskSleep = pdMS_TO_TICKS( 100 );
	business_logic::Communication::CommunicationManager* commMng = reinterpret_cast<business_logic::Communication::CommunicationManager*>(argument);
#ifdef ASSEMBLER_TASK
	commMng->initialization(taskHandlerImgAssembler);
#endif

    size_t freeHeapSize = xPortGetFreeHeapSize();
    size_t minEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();
    const std::string startMsg = "SystemTasks::communicationTask started --> freeHeapSize: " + std::to_string(freeHeapSize) + " minEverFreeHeapSize " + std::to_string(minEverFreeHeapSize);
    LOG_INFO(startMsg);
  /* Infinite loop */
    uint32_t startExecutionTime;
	while(1)
	{

		LOG_TRACE("SystemTasks::communicationTask triggered");
		RunTimeStats_Start(&startExecutionTime);
		const auto t1 = xTaskGetTickCount();
		static uint32_t loopIndex = 0;
		if(loopIndex > 500000000)
		{
			logMemoryUsage();
			loopIndex = 0;
		}
		const auto result = commMng->receiveData();
		loopIndex++;
		const auto executionTime = (xTaskGetTickCount() - t1) * portTICK_PERIOD_MS;
		if(result)LOG_DEBUG("SystemTasks::communicationTask executed in: ", executionTime, " ms");
		RunTimeStats_End("communicationTask", startExecutionTime, false);
		LOG_DEBUG("SystemTasks::communicationTask going to sleep");

		vTaskDelay( taskSleep );
		LOG_DEBUG("SystemTasks::communicationTask wakeup");
	}
}
	
void SystemTasksManager::image3dCapturerTask(void* argument)
{
	LOG_INFO("Starting image3dCapturerTask");
	//business_logic::ImageCapturer3D* image3dCapturer = reinterpret_cast<business_logic::ImageCapturer3D*>(argument);

	const TickType_t taskSleep = pdMS_TO_TICKS( 5000 );
	m_image3DCapturer->initialize();
  /* Infinite loop */
	uint32_t startExecutionTime;
	while(1)
	{
		RunTimeStats_Start(&startExecutionTime);
		const auto t1 = xTaskGetTickCount();
		static uint8_t captureId = 0;
		try
		{

			logMemoryUsage();
			m_lastCaptureTimestampStart = m_globalClkMng->getAbsotuleTime();
			const auto imageSize = m_image3DCapturer->captureImage();

			auto last3dSample = m_image3DCapturer->getLastCapture();
			//m_lastCaptureTimestampEnd = m_globalClkMng->getAbsotuleTime();
			//const auto captureDeltaTime = m_lastCaptureTimestampEnd - m_lastCaptureTimestampStart;
			//LOG_TRACE("Start capture at ", std::to_string(m_lastCaptureTimestampStart));
			//LOG_TRACE("End capture at ", std::to_string(m_lastCaptureTimestampEnd));

			//LOG_DEBUG("3D image captured in ", std::to_string(captureDeltaTime), " ns");
			//business_logic::LidarArray* pxPointerToxMessage;
			//pxPointerToxMessage = &last3dSample;
			int cnt = 0;
			LOG_DEBUG("SystemTasks::image3dCapturerTask START_FRAME");
			for(auto sample : last3dSample)
			{
				std::string sample3D = std::to_string(sample.m_angleServoH) + "," + std::to_string(sample.m_angleServoV) + "," + std::to_string(sample.m_pointDistance);
				LOG_INFO(sample3D);
				cnt++;
				if(cnt >= imageSize) break;
			}
			LOG_DEBUG("SystemTasks::image3dCapturerTask END_FRAME");
//			m_capturesQueue->sendToBack(( void * ) &pxPointerToxMessage);
			captureId++;
			const auto executionTime = (xTaskGetTickCount() - t1) * portTICK_PERIOD_MS;
			LOG_DEBUG("SystemTasks::image3dCapturerTask executed in: ", executionTime, " ms");
			RunTimeStats_End("image3dCapturerTask", startExecutionTime);
			vTaskDelay( taskSleep );
		}
		catch (const services::BaseException& e)
		{
			LOG_ERROR("Exception [", e.getErrorId() ,"]while captureImage: ", e.what());
		}
	}  
}
void SystemTasksManager::imageClassificationTask(void* argument)
{

	LOG_INFO("Starting imageClassificationTask");
	m_imageClassifier->initialization();
	const TickType_t taskSleep = pdMS_TO_TICKS( 5000 );
  /* Infinite loop */

	while(1)
	{
		m_imageClassifier->detect();
		vTaskDelay( taskSleep );
	}
}

void SystemTasksManager::imageClassificationTask(void* argument)
{

	LOG_INFO("SystemTasksManager::imageClassificationTask started");
	m_imageClassifier->initialization();
	LOG_INFO("SystemTasksManager::imageClassificationTask initialization done");

  /* Infinite loop */
	int32_t triggerSystemStats = 0;
	uint32_t startExecutionTime;
	while(1)
	{
		RunTimeStats_Start(&startExecutionTime);
		LOG_INFO("SystemTasksManager::imageClassificationTask waiting for image ready");
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
		LOG_INFO("SystemTasksManager::imageClassificationTask image ready");
		const auto t1 = xTaskGetTickCount();
		logMemoryUsage();
		m_imageClassifier->performInference();
		logMemoryUsage();
		const auto executionTime = (xTaskGetTickCount() - t1) * portTICK_PERIOD_MS;
		LOG_DEBUG("SystemTasks::imageClassificationTask executed in: ", executionTime, " ms");

		RunTimeStats_End("imageClassificationTask", startExecutionTime, false);
	}
}


void SystemTasksManager::sensorFusionTask(void* argument)
{

	LOG_INFO("SystemTasksManager::sensorFusionTask started");
	//m_imageClassifier->initialization();
	LOG_INFO("SystemTasksManager::sensorFusionTask initialization done");
	const TickType_t taskSleep = pdMS_TO_TICKS( 5000 );
  /* Infinite loop */
	uint32_t startExecutionTime;
	while(1)
	{
		RunTimeStats_Start(&startExecutionTime);
		//Waiting for imageReady
		LOG_DEBUG("SystemTasksManager::sensorFusionTask waiting for image ready");
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
		LOG_INFO("SystemTasksManager::sensorFusionTask image ready");
		const auto t1 = xTaskGetTickCount();
		logMemoryUsage();
		//TODO perform sensor fusion process
		logMemoryUsage();
		const auto executionTime = (xTaskGetTickCount() - t1) * portTICK_PERIOD_MS;
		LOG_DEBUG("SystemTasks::sensorFusionTask executed in: ", executionTime, " ms");
		RunTimeStats_End("sensorFusionTask", startExecutionTime);
		vTaskDelay( taskSleep );
	}
}

void SystemTasksManager::imageAssemblerTask(void* argument)
{

	LOG_INFO("SystemTasksManager::imageAssemblerTask started");
	m_imageAssembler->initialization(taskHandlerSensorFusion, taskHandlerImgClassifier);
	LOG_INFO("SystemTasksManager::imageAssemblerTask initialization done");
  /* Infinite loop */
	uint32_t startExecutionTime;
	while(1)
	{
		RunTimeStats_Start(&startExecutionTime);
		LOG_TRACE("SystemTasksManager::imageAssemblerTask waiting for notification");
		uint32_t ulNotificationValue = 0;
		xTaskNotifyWait( 0, 0x00, &ulNotificationValue, portMAX_DELAY );
		uint8_t msgIndex    = (ulNotificationValue >> 16) & 0xFF;
		uint8_t cborIndex   = (ulNotificationValue >> 8)  & 0xFF;
		bool isEndOfImage   = ulNotificationValue & 0x1;

		const std::string strMsg = "SystemTasksManager::imageAssemblerTask NOTIFICATION receive: msgIndex: " + std::to_string(msgIndex) + "-- cborIndex: " + std::to_string(cborIndex) + " -- isEndOfFrame: " + std::to_string(isEndOfImage);
		LOG_DEBUG(strMsg);
		const auto t1 = xTaskGetTickCount();
		logMemoryUsage();
		static TickType_t lastEndOfImageTick = 0;
		const TickType_t currentTick = xTaskGetTickCount();
		if (isEndOfImage)
		{
		    TickType_t deltaTicks = currentTick - lastEndOfImageTick;
		    uint32_t deltaTimeMs = deltaTicks * portTICK_PERIOD_MS;
		    lastEndOfImageTick = currentTick;

		    const std::string strDelta = "DeltaTime between assembled frames: " + std::to_string(deltaTimeMs) + " ms";
		    LOG_DEBUG(strDelta);
		}
		const auto t12 = xTaskGetTickCount();
		LOG_DEBUG("SystemTasksManager::imageAssemblerTask assembling frame");
		m_imageAssembler->assembleFrame(msgIndex, cborIndex, isEndOfImage);
		logMemoryUsage();

		RunTimeStats_End("imageAssemblerTask", startExecutionTime, false);
		const auto executionTime = (xTaskGetTickCount() - t1) * portTICK_PERIOD_MS;
		LOG_DEBUG("SystemTasks::imageAssemblerTask executed in: ", executionTime );
		//static const TaskHandle_t taskToNotify = taskHandlerCommunication;
		//xTaskNotifyGive(taskToNotify);
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

	LOG_INFO("SystemTasksManager::createPoolTasks started");
#ifdef IMAGE_CLASSIFIER_NODE
	m_imageClassifierTaskHandler  = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::imageClassificationTask, "imageClassificationTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(m_imageClassifier.get()), 2048);
	//m_sensorFusionTaskHandler  = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::sensorFusionTask, "sensorFusionTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(nullptr), 2048);
	taskHandlerSensorFusion = m_sensorFusionTaskHandler->getTaskHandler();;
	taskHandlerImgClassifier = m_imageClassifierTaskHandler->getTaskHandler();;
	m_clockSyncTaskHandler       = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::globalClockSyncronization, "GlobalClockSyncronization", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(m_globalClkMng.get()), 1024);
#ifdef ASSEMBLER_TASK
	m_imageAssemblerTaskHandler  = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::imageAssemblerTask, "imageAssemblerTask", DefaultPriorityTask + 2, static_cast<business_logic::Osal::VoidPtr>(m_imageAssembler.get()), 2048);
	taskHandlerImgAssembler = m_imageAssemblerTaskHandler->getTaskHandler();
#endif
	m_commTaskHandler            = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::communicationTask, "CommunicationTask", DefaultPriorityTask +1 , static_cast<business_logic::Osal::VoidPtr>(m_commMng.get()), 2048);
	taskHandlerCommunication = m_commTaskHandler->getTaskHandler();
#else
	m_image3dCapturerTaskHandler = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasksManager::image3dCapturerTask, "image3dCapturerTask", DefaultPriorityTask + 1, /*static_cast<business_logic::Osal::VoidPtr>(m_image3DCapturer.get())*/(void*)1, 4096);
#endif
	LOG_INFO("SystemTasksManager::createPoolTasks done");

}

}
