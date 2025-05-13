#include <business_logic/SensorFusion/FusionManager.h>

namespace business_logic
{
namespace SensorFusion
{
FusionManager::FusionManager(Osal::StreamBufferHandler* input, Osal::StreamBufferHandler* output) : frameInput(input), syncFrames(output)
{

}


FusionManager::~FusionManager()
{

}

void FusionManager::initialization()
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
}
