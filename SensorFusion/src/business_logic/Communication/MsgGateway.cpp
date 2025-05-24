
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

MsgGateway::MsgGateway()
{
	uint32_t queueItemSize   = sizeof(std::array<uint8_t, hardware_abstraction::Controllers::CAN_DATA_PAYLOAD_SIZE>);
	uint32_t queueLength     = 200;
	m_cameraFramesQueue = std::make_shared<business_logic::Osal::QueueHandler>(queueLength, queueItemSize);
	m_lidarFramesQueue  = std::make_shared<business_logic::Osal::QueueHandler>(queueLength, queueItemSize);
	m_imageAssembler    = std::make_shared<business_logic::ImageClassifier::ImageAssembler>(m_cameraFramesQueue);
}
void MsgGateway::initialization()
{

}

void MsgGateway::storeMsg(const uint8_t frameId, const std::array<uint8_t, hardware_abstraction::Controllers::CAN_DATA_PAYLOAD_SIZE>& frame)
{
	//hardware_abstraction::Controllers::CanFrame* newFrame;
	//newFrame = &frame;
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
			m_imageAssembler->assembleFrame(msgIndex, cborIndex, isEndOfImage);
			break;
		}
		case static_cast<uint8_t>(CAN_MSG_TYPES::LIDAR_3D_IMAGE):
			LOG_INFO("LIDAR_3D_IMAGE frame: ", std::to_string(msgIndex), " completed");
			break;
		default:
			LOG_INFO("Unknown frame completed");
			break;
	}
}
}
}
