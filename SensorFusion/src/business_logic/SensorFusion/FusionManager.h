#pragma once

#include <business_logic/Osal/StreamBufferHandler.h>

namespace business_logic
{
namespace SensorFusion
{
class FusionManager
{
private:
	Osal::StreamBufferHandler*  frameInput;
	Osal::StreamBufferHandler*  syncFrames;

public:
	FusionManager(Osal::StreamBufferHandler* input, Osal::StreamBufferHandler* output);
	virtual ~FusionManager();

	void pullFrames();
	void filterFrames();
	void fusionFrames();
};
}
}

