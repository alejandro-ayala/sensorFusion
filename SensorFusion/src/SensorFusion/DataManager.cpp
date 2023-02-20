#include "DataManager.h"

namespace SensorFusion
{
DataManager::DataManager(RTOS::StreamBufferHandler* inputBuffer, RTOS::StreamBufferHandler* outputBuffer) : inputMsg(inputBuffer), frameBuffer(outputBuffer)
{

}


DataManager::~DataManager()
{

}


void DataManager::getInputData()
{

}

bool DataManager::checkTimeSyncronization()
{

}

void DataManager::pushOutputFrames()
{

}


}
