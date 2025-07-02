#pragma once
#include "business_logic/ImageClassifier/ImageProvider.h"
#include "business_logic/Osal/QueueHandler.h"
#include <memory>
#include <vector>
#include <cstdint>
namespace business_logic
{
namespace ImageAssembler
{
class ImageAssembler
{
private:
	std::shared_ptr<business_logic::Osal::QueueHandler> m_cameraFramesQueue;
	std::shared_ptr<business_logic::Osal::QueueHandler> m_cameraSnapshotsQueue;
	TaskHandle_t m_taskHandlerSensorFusion;
	TaskHandle_t m_taskHandlerImgClassifier;
	std::shared_ptr<business_logic::ImageClassifier::ImageProvider> m_imageProvider;

public:
    ImageAssembler(const std::shared_ptr<business_logic::Osal::QueueHandler>& cameraFramesQueue, const std::shared_ptr<business_logic::ImageClassifier::ImageProvider>& imageProvider);
    ~ImageAssembler() = default;

    void initialization(const TaskHandle_t& taskHandlerSensorFusion, const TaskHandle_t& taskHandlerImgClassifier);
    bool loadData(const std::vector<uint8_t>& input);
    bool assembleImage(uint8_t imageId, uint8_t totalChunks);
    bool assembleFrame(uint8_t msgIndex, uint8_t cborIndex, bool isEndOfImage = false);
};

}
}
