#pragma once
#include "business_logic/Osal/QueueHandler.h"
#include <memory>
#include <vector>
#include <cstdint>
namespace business_logic
{
namespace ImageClassifier
{
class ImageAssembler
{
private:
	std::shared_ptr<business_logic::Osal::QueueHandler> m_cameraFramesQueue;
	std::shared_ptr<business_logic::Osal::QueueHandler> m_cameraSnapshotsQueue;


public:
    ImageAssembler(const std::shared_ptr<business_logic::Osal::QueueHandler>& cameraFramesQueue);
    ~ImageAssembler() = default;

    bool loadData(const std::vector<uint8_t>& input);
    bool assembleImage(uint8_t imageId, uint8_t totalChunks);
    bool assembleFrame(uint8_t msgIndex, uint8_t cborIndex, bool isEndOfImage = false);
};

}
}
