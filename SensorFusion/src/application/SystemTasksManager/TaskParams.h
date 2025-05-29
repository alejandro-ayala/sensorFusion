#pragma once

#include "business_logic/ClockSyncronization/TimeBaseManager.h"
#include "business_logic/ImageCapturer3D/ImageCapturer3D.h"
#include "business_logic/ImageAssembler/SharedImage.h"
namespace application
{
struct TaskParams
{
	std::unique_ptr<business_logic::ImageCapturer3D> image3dCapturer;
	std::shared_ptr<business_logic::ClockSyncronization::TimeBaseManager> globalClkMng;
	std::shared_ptr<business_logic::Communication::CommunicationManager> commMng;
	std::shared_ptr<business_logic::Osal::QueueHandler> cameraFramesQueue;
	std::shared_ptr<business_logic::ImageClassifier::ImageProvider> imageProvider;
};

}
