#pragma once

#include "business_logic/ClockSyncronization/SharedClockSlaveManager.h"
#include "business_logic/ImageCapturer3D/ImageCapturer3D.h"

namespace application
{
struct TaskParams
{
	std::shared_ptr<business_logic::ImageCapturer3D> image3dCapturer;
	std::shared_ptr<business_logic::ClockSyncronization::SharedClockSlaveManager> globalClkMng;
	std::shared_ptr<business_logic::Communication::CommunicationManager> commMng;
};

}
