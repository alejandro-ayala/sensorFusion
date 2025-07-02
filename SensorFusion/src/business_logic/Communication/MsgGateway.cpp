
#include "MsgGateway.h"
#include "business_logic/DataSerializer/DataSerializer.h"
#include "business_logic/DataSerializer/ImageSnapshot.h"
#include "services/Logger/LoggerMacros.h"
#include <string>
#include <vector>

namespace business_logic
{
namespace Communication
{

enum class CAN_MSG_TYPES
{
	CAMERA_IMAGE = 0x18,
	LIDAR_3D_IMAGE  = 0x19
};

#ifndef ASSEMBLER_TASK
MsgGateway::MsgGateway(const std::shared_ptr<business_logic::Osal::QueueHandler>& cameraFramesQueue, const std::shared_ptr<business_logic::ImageAssembler::ImageAssembler>& imageAssembler) : m_cameraFramesQueue(cameraFramesQueue), m_imageAssembler(imageAssembler)
#else
MsgGateway::MsgGateway(const std::shared_ptr<business_logic::Osal::QueueHandler>& cameraFramesQueue) : m_cameraFramesQueue(cameraFramesQueue)
#endif
{
	uint32_t queueItemSize   = sizeof(std::array<uint8_t, hardware_abstraction::Controllers::CAN_DATA_PAYLOAD_SIZE>);
	uint32_t queueLength     = 200;
	m_lidarFramesQueue  = std::make_shared<business_logic::Osal::QueueHandler>(queueLength, queueItemSize);

}

void MsgGateway::initialization(const TaskHandle_t& taskToNotify)
{
	m_taskToNotify = taskToNotify;
}

void MsgGateway::storeMsg(const uint8_t frameId, const std::array<uint8_t, hardware_abstraction::Controllers::CAN_DATA_PAYLOAD_SIZE>& frame)
{

	switch (frameId)
	{
		case static_cast<uint8_t>(CAN_MSG_TYPES::CAMERA_IMAGE):
			m_cameraFramesQueue->sendToBack(frame.data());
			break;
		case static_cast<uint8_t>(CAN_MSG_TYPES::LIDAR_3D_IMAGE):
			m_lidarFramesQueue->sendToBack(frame.data());
			break;
		default:
			break;
	}
}

void MsgGateway::completedFrame(uint16_t msgType, uint8_t msgIndex, uint8_t cborIndex, bool isEndOfImage)
{
	static uint8_t imageId = 0;
	switch (msgType)
	{
		case static_cast<uint8_t>(CAN_MSG_TYPES::CAMERA_IMAGE):
		{
			//TODO notify to assembleImageTask
			//m_imageAssembler->assembleFrame(msgIndex, cborIndex, isEndOfImage);
			const std::string strMsg = "MsgGateway::completedFrame CAMERA_IMAGE. msgIndex: " + std::to_string(msgIndex) + "-- cborIndex: " + std::to_string(cborIndex) + " -- isEndOfFrame: " + std::to_string(isEndOfImage & 0x1);
			LOG_DEBUG(strMsg);
#ifdef ASSEMBLER_TASK
			uint32_t valueToNotify = (msgIndex << 16) | (cborIndex << 8) | (isEndOfImage & 0x1);
			const auto t1 = xTaskGetTickCount();
			xTaskNotify(m_taskToNotify, valueToNotify, eSetValueWithOverwrite);
#else
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
			m_imageAssembler->assembleFrame(msgIndex, cborIndex, isEndOfImage);
			logMemoryUsage();

			const auto executionTime = (xTaskGetTickCount() - t1) * portTICK_PERIOD_MS;
			LOG_TRACE("SystemTasks::imageAssemblerTask executed in: ", executionTime, " ms but m_imageAssembler->assembleFrame in: ", ((xTaskGetTickCount() - t12) * portTICK_PERIOD_MS) );

#endif
			//LOG_TRACE("MsgGateway::completedFrame notification done waiting to finish assemble frame");
			//ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
			//const auto assembledFrameTime = (xTaskGetTickCount() - t1) * portTICK_PERIOD_MS;
			//LOG_DEBUG("MsgGateway::completedFrame assemble frame DONE in ", assembledFrameTime);
			break;
		}
		case static_cast<uint8_t>(CAN_MSG_TYPES::LIDAR_3D_IMAGE):
			LOG_INFO("LIDAR_3D_IMAGE frame: ", std::to_string(msgIndex), " completed");
			break;
		default:
			LOG_WARNING("Unknown frame completed");
			break;
	}
}
}
}
