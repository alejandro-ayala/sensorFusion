#include "FusionManager.h"

namespace SensorFusion
{
FusionManager::FusionManager(RTOS::StreamBufferHandler* input, RTOS::StreamBufferHandler* output) : frameInput(input), syncFrames(output)
{

}


FusionManager::~FusionManager()
{

}


void FusionManager::pullFrames()
{

}

void FusionManager::filterFrames()
{

}

void FusionManager::fusionFrames()
{

}


}
