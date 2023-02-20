#pragma once

#include "RTOS/StreamBufferHandler.h"

namespace SensorFusion
{
class DataManager
{
private:
	RTOS::StreamBufferHandler*  inputMsg;
	RTOS::StreamBufferHandler*  frameBuffer;
public:
	DataManager(RTOS::StreamBufferHandler* inputBuffer, RTOS::StreamBufferHandler* outputBuffer);
	virtual ~DataManager();

	void getInputData();
	bool checkTimeSyncronization();
	void pushOutputFrames();
};
}

