#pragma once

#include "RTOS/StreamBufferHandler.h"

namespace Components
{
namespace SensorFusion
{
class FusionManager
{
private:
	RTOS::StreamBufferHandler*  frameInput;
	RTOS::StreamBufferHandler*  syncFrames;

public:
	FusionManager(RTOS::StreamBufferHandler* input, RTOS::StreamBufferHandler* output);
	virtual ~FusionManager();

	void pullFrames();
	void filterFrames();
	void fusionFrames();
};
}
}

