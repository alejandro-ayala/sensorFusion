#pragma once

#include "business_logic/Osal/QueueHandler.h"
#include "hardware_abstraction/Controllers/CAN/CanFrame.h"
#include "business_logic/ImageClassifier/ImageAssembler.h"
#include <memory>
namespace business_logic
{
namespace Communication
{

class MsgGateway
{
private:
	std::shared_ptr<business_logic::Osal::QueueHandler> m_cameraFramesQueue;
	std::shared_ptr<business_logic::Osal::QueueHandler> m_lidarFramesQueue;
	std::shared_ptr<business_logic::ImageClassifier::ImageAssembler> m_imageAssembler;
public:
	MsgGateway();
	virtual ~MsgGateway() = default;

	void initialization();
	void storeMsg(const uint8_t frameId, const std::array<uint8_t, hardware_abstraction::Controllers::CAN_DATA_PAYLOAD_SIZE>& frame);
	void completedFrame(uint16_t msgType, uint8_t msgIndex, uint8_t cborIndex);
};
}
}
