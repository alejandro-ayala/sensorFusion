
#include "MsgGateway.h"
#include "services/Logger/LoggerMacros.h"

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
	uint32_t queueItemSize   = sizeof(hardware_abstraction::Controllers::CanFrame*);
	uint32_t queueLength     = 200;
	m_cameraFramesQueue = std::make_shared<business_logic::Osal::QueueHandler>(queueLength, queueItemSize);
	m_lidarFramesQueue = std::make_shared<business_logic::Osal::QueueHandler>(queueLength, queueItemSize);
}
void MsgGateway::initialization()
{

}

void MsgGateway::storeMsg(const hardware_abstraction::Controllers::CanFrame& frame)
{
	hardware_abstraction::Controllers::CanFrame* newFrame;
	newFrame = &frame;
	switch (frame.id)
	{
		case static_cast<uint8_t>(CAN_MSG_TYPES::CAMERA_IMAGE):
			m_cameraFramesQueue->sendToBack(( void * ) &newFrame);
			break;
		case static_cast<uint8_t>(CAN_MSG_TYPES::LIDAR_3D_IMAGE):
			m_lidarFramesQueue->sendToBack(( void * ) &newFrame);
			break;
		default:
			break;
	}
}
void MsgGateway::completedFrame(uint16_t msgType, uint8_t msgIndex, uint8_t cborIndex)
{
	switch (msgType)
	{
		case static_cast<uint8_t>(CAN_MSG_TYPES::CAMERA_IMAGE):
		{
			LOG_INFO("CAMERA_IMAGE frame: ", std::to_string(msgIndex), " completed with ", std::to_string(cborIndex), " cbor msgs");
			const auto storedMsg = m_cameraFramesQueue->getStoredMsg();
			LOG_INFO("Stored frames in CameraImageQueue: ", std::to_string(storedMsg));
			for(size_t idx = 0; idx < storedMsg; idx++)
			{
				hardware_abstraction::Controllers::CanFrame* rxBuffer;
				m_cameraFramesQueue->receive((void*&)rxBuffer);
				LOG_INFO("Read frame: ", std::to_string(rxBuffer->id), " index: ", std::to_string(rxBuffer->data[0]), "--", std::to_string(rxBuffer->data[1]));
				const auto storedMsg = m_cameraFramesQueue->getStoredMsg();
				LOG_INFO("Stored frames in CameraImageQueue: ", std::to_string(storedMsg));
			}
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
