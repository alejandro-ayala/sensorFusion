#pragma once
#include <business_logic/ClockSyncronization/TimeController.h>
#include <business_logic/ClockSyncronization/TimeStamp.h>
#include <business_logic/Communication/IData.h>
#include <hardware_abstraction/Controllers/CAN/PsCanController.h>
#include "MsgGateway.h"
#include <memory>
#include <vector>
#include "CanMsg.h"

namespace business_logic
{
namespace Communication
{
class CommunicationManager
{
private:
	std::shared_ptr<ClockSyncronization::TimeController> timeController;
	std::shared_ptr<hardware_abstraction::Controllers::PsCanController> canController;
	std::shared_ptr<MsgGateway> msgGateway;

	ClockSyncronization::TimeStamp globalTimeStamp;
public:
#ifdef ASSEMBLER_TASK
	CommunicationManager(const std::shared_ptr<ClockSyncronization::TimeController>& timecontroller, const std::shared_ptr<hardware_abstraction::Controllers::PsCanController>& cancontroller, const std::shared_ptr<business_logic::Osal::QueueHandler>& cameraFramesQueue);
#else
	CommunicationManager(const std::shared_ptr<ClockSyncronization::TimeController>& timecontroller, const std::shared_ptr<hardware_abstraction::Controllers::PsCanController>& cancontroller, const std::shared_ptr<business_logic::Osal::QueueHandler>& cameraFramesQueue, const std::shared_ptr<business_logic::ImageAssembler::ImageAssembler>& imageAssembler);
#endif
	virtual ~CommunicationManager();

	void initialization(const TaskHandle_t& taskToNotify);
	bool sendData(IData msg);
	bool sendData(const std::vector<business_logic::Communication::CanMsg>& dataToSend);
	bool receiveData();
	bool selfTest();

};
}
}
