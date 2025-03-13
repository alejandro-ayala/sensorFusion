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
public:
	MsgGateway();
	virtual ~MsgGateway() = default;

	void initialization();
	void storeMsg(const hardware_abstraction::Controllers::CanFrame& frame);
	void completedFrame(uint16_t msgType, uint8_t msgIndex, uint8_t cborIndex);
};
}
}
