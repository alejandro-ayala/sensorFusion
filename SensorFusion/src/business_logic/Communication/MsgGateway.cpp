
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
	m_lidarFramesQueue = std::make_shared<business_logic::Osal::QueueHandler>(queueLength, queueItemSize);
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
void MsgGateway::completedFrame(uint16_t msgType, uint8_t msgIndex, uint8_t cborIndex)
{
	switch (msgType)
	{
		case static_cast<uint8_t>(CAN_MSG_TYPES::CAMERA_IMAGE):
		{
			LOG_TRACE("CAMERA_IMAGE frame: ", std::to_string(msgIndex), " completed with ", std::to_string(cborIndex), " cbor msgs");
			const auto storedMsg = m_cameraFramesQueue->getStoredMsg();
			std::vector<uint8_t> cborFrame;
			cborFrame.reserve(storedMsg * hardware_abstraction::Controllers::CAN_DATA_PAYLOAD_SIZE);

			for(size_t idx = 0; idx < storedMsg; idx++)
			{
				std::array<uint8_t, hardware_abstraction::Controllers::CAN_DATA_PAYLOAD_SIZE> rxBuffer;
				m_cameraFramesQueue->receive(rxBuffer.data());
				cborFrame.insert(cborFrame.end(), rxBuffer.begin() + 2, rxBuffer.end());
//				const std::string stringBuffer =  std::to_string(rxBuffer[0]) + " " + std::to_string(rxBuffer[1]) + " " + std::to_string(rxBuffer[2]) + " " + std::to_string(rxBuffer[3]) + " " + std::to_string(rxBuffer[4]) + " " + std::to_string(rxBuffer[5]) + " " + std::to_string(rxBuffer[6]) + " " + std::to_string(rxBuffer[7]);
//				LOG_DEBUG("Read frame: ", stringBuffer);
//
//				std::vector<uint8_t> insertedData;
//				insertedData.insert(insertedData.end(), rxBuffer.begin() + 2, rxBuffer.end());
//				std::string insertedDataStr;
//				for(const auto& data : insertedData)
//					insertedDataStr += std::to_string(data) + " ";
//
//				LOG_DEBUG("Inserted data: ", insertedDataStr);
			}
			const auto pendingStoredMsg = m_cameraFramesQueue->getStoredMsg();
			if(pendingStoredMsg != 0)
				LOG_WARNING("CAMERA_IMAGE frame: pending frames after read queue: ", std::to_string(pendingStoredMsg));


		    // Eliminar 0xFF's incluidos para completar el frame de 8 bytes
		    auto it = std::find_if(cborFrame.rbegin(), cborFrame.rend(), [](uint8_t val) { return val != 0xFF; });
		    cborFrame.erase(it.base(), cborFrame.end());

//			std::string cborStr;
//			for(const auto& element : cborFrame)
//				cborStr += std::to_string(element) + " ";
//			LOG_INFO(cborStr);

			try
			{
				auto m_dataSerializer = std::make_shared<business_logic::DataSerializer>();
				business_logic::ImageSnapshot cborImgChunk;
				m_dataSerializer->deserialize(cborImgChunk, cborFrame);
				std::string cborImgChunkStr = "Deserialized ImageSnapshot: " + std::to_string(cborImgChunk.m_msgId) +  "- "+ std::to_string(cborImgChunk.m_msgIndex) + " of " + std::to_string(cborImgChunk.m_imgSize) + " bytes at: " + std::to_string(cborImgChunk.m_timestamp);
				LOG_TRACE(cborImgChunkStr);
				std::string cborStr = "[" + std::to_string(cborImgChunk.m_msgId) +  "- "+ std::to_string(cborImgChunk.m_msgIndex) + "]";
				for(int i = 0; i< cborImgChunk.m_imgSize;i++)
					cborStr += std::to_string(cborImgChunk.m_imgBuffer[i]) + " ";
				LOG_INFO(cborStr);
			}
			catch (const std::exception& e) {
			    LOG_ERROR("Exception caught: " + std::string(e.what()));
			}
			catch (...) {
				LOG_ERROR("Unknown exception caught during image deserialization.");
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
