#pragma once

#include "RTOS/StreamBufferHandler.h"

namespace Components
{
namespace SensorFusion
{
class MatchingManager
{
private:
	RTOS::StreamBufferHandler*  frameInput;
	RTOS::StreamBufferHandler*  syncFrames;

public:
	MatchingManager(RTOS::StreamBufferHandler* input, RTOS::StreamBufferHandler* output);
	virtual ~MatchingManager();

	void getInputData();
	void transformCoordinates();
	void matchingFrames();
};
}
}

