#pragma once

#include <business_logic/Osal/StreamBufferHandler.h>

namespace business_logic
{
namespace SensorFusion
{
class MatchingManager
{
private:
	Osal::StreamBufferHandler*  frameInput;
	Osal::StreamBufferHandler*  syncFrames;

public:
	MatchingManager(Osal::StreamBufferHandler* input, Osal::StreamBufferHandler* output);
	virtual ~MatchingManager();

	void getInputData();
	void transformCoordinates();
	void matchingFrames();
};
}
}

