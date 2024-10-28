#include <business_logic/SensorFusion/DataManager.h>

namespace business_logic
{
namespace SensorFusion
{
DataManager::DataManager(Osal::StreamBufferHandler* inputBuffer, Osal::StreamBufferHandler* outputBuffer) : inputMsg(inputBuffer), frameBuffer(outputBuffer)
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
}
