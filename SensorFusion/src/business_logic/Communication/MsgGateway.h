#pragma once

#include "business_logic/Osal/QueueHandler.h"
#include "hardware_abstraction/Controllers/CAN/CanFrame.h"
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
	TaskHandle_t m_taskToNotify;

public:
	explicit MsgGateway(const std::shared_ptr<business_logic::Osal::QueueHandler>& cameraFramesQueue);
	virtual ~MsgGateway() = default;

	void initialization(const TaskHandle_t& taskToNotify);
	void storeMsg(const uint8_t frameId, const std::array<uint8_t, hardware_abstraction::Controllers::CAN_DATA_PAYLOAD_SIZE>& frame);
	void completedFrame(uint16_t msgType, uint8_t msgIndex, uint8_t cborIndex, bool isEndOfImage = false);
};
}
}
