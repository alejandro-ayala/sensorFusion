#pragma once

#include <business_logic/Osal/StreamBufferHandler.h>

namespace business_logic
{
namespace SensorFusion
{
class DataManager
{
private:
	Osal::StreamBufferHandler*  inputMsg;
	Osal::StreamBufferHandler*  frameBuffer;
public:
	DataManager(Osal::StreamBufferHandler* inputBuffer, Osal::StreamBufferHandler* outputBuffer);
	virtual ~DataManager();

	void getInputData();
	bool checkTimeSyncronization();
	void pushOutputFrames();
};
}
}

