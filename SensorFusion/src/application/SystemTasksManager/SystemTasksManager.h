#pragma once
#include "business_logic/Osal/TaskHandler.h"
#include "business_logic/Communication/CommunicationManager.h"
#include "business_logic/ImageCapturer3D/ImageCapturer3D.h"
#include "business_logic/ImageClassifier/ImageClassifier.h"
#include "business_logic/ImageClassifier/ImageAssembler.h"
#include "business_logic/ClockSyncronization/TimeBaseManager.h"
#include "TaskParams.h"
#include "business_logic/Communication/CanMsg.h"
#include <memory>
#include <vector>

namespace business_logic
{
class Image3DSnapshot;
}

namespace application
{

constexpr uint32_t DefaultPriorityTask = 0;
class SystemTasksManager
{
private:

	TaskParams m_taskParam;
	static inline std::unique_ptr<business_logic::ImageCapturer3D> m_image3DCapturer;
	static inline std::shared_ptr<business_logic::ClockSyncronization::TimeBaseManager> m_globalClkMng;
	std::shared_ptr<business_logic::Communication::CommunicationManager> m_commMng;
	static inline std::shared_ptr<business_logic::ImageClassifier::ImageClassifierManager> m_imageClassifier;
	static inline std::shared_ptr<business_logic::ImageClassifier::ImageAssembler> m_imageAssembler;


	static inline uint64_t m_lastCaptureTimestampStart = 0;
	static inline uint64_t m_lastCaptureTimestampEnd = 0;

	static inline std::shared_ptr<business_logic::Osal::TaskHandler> m_clockSyncTaskHandler;
	static inline std::shared_ptr<business_logic::Osal::TaskHandler> m_image3dCapturerTaskHandler;
	static inline std::shared_ptr<business_logic::Osal::TaskHandler> m_commTaskHandler;
	static inline std::shared_ptr<business_logic::Osal::TaskHandler> m_imageClassifierTaskHandler;
	static inline std::shared_ptr<business_logic::Osal::TaskHandler> m_imageAssemblerTaskHandler;
	static inline std::shared_ptr<business_logic::Osal::TaskHandler> m_sensorFusionTaskHandler;
	static inline std::shared_ptr<business_logic::Osal::QueueHandler> m_capturesQueue;

public:
	SystemTasksManager(TaskParams&& systemTaskMngParams);
	virtual ~SystemTasksManager() = default;
	static void globalClockSyncronization(void* argument);
	static void communicationTask(void* argument);
	static void image3dCapturerTask(void* argument);
	static void imageClassificationTask(void* argument);
	static void imageAssemblerTask(void* argument);
	static void sensorFusionTask(void* argument);
    static void getNextImage(std::array<business_logic::LidarPoint, business_logic::IMAGE3D_SIZE>& lastCapture);
    static bool isPendingData();
    static void splitCborToCanMsgs(uint8_t canMsgId, const std::vector<uint8_t>& cborSerializedChunk, std::vector<business_logic::Communication::CanMsg>& canMsgChunks);
    void createPoolTasks();
};

}
