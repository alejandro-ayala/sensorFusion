#include "ImageAssembler.h"
#include "business_logic/DataSerializer/DataSerializer.h"
#include "business_logic/DataSerializer/ImageSnapshot.h"
#include "hardware_abstraction/Controllers/CAN/CanFrame.h"
#include "services/Logger/LoggerMacros.h"

namespace business_logic
{
namespace ImageClassifier
{
ImageAssembler::ImageAssembler(const std::shared_ptr<business_logic::Osal::QueueHandler>& cameraFramesQueue) : m_cameraFramesQueue(cameraFramesQueue)
{
	uint32_t queueItemSize   = sizeof(std::shared_ptr<business_logic::ImageSnapshot>);
	uint32_t queueLength     = 50;
	m_cameraSnapshotsQueue = std::make_shared<business_logic::Osal::QueueHandler>(queueLength, queueItemSize);
}


bool ImageAssembler::loadData(const std::vector<uint8_t>& input)
{
	return true;
}

bool ImageAssembler::assembleImage(uint8_t imageId, uint8_t totalChunks)
{
	std::vector<uint8_t> assembledImage;
	const auto storedCborChunks = m_cameraSnapshotsQueue->getStoredMsg();
	if(storedCborChunks != totalChunks)
	{
		const std::string msg = "Queue contains " + std::to_string(storedCborChunks) + " and the image to assembly expects " + std::to_string(totalChunks) ;
		LOG_WARNING(msg);
	}
	else
	{
		const std::string cborImgChunkStr = "Completed ImageSnapshot: " + std::to_string(imageId) + " with " + std::to_string(totalChunks) + " chunks ";
		LOG_INFO(cborImgChunkStr);
	}
	for(uint32_t idx = 0; idx < storedCborChunks; idx++)
	{
		auto rxSnapshotChunk = std::make_shared<business_logic::ImageSnapshot>();
        m_cameraSnapshotsQueue->receive(static_cast<void*>(rxSnapshotChunk.get()));
        assembledImage.insert(assembledImage.end(), rxSnapshotChunk->m_imgBuffer.get(), rxSnapshotChunk->m_imgBuffer.get() + rxSnapshotChunk->m_imgSize);
	}

	return true;
}

bool ImageAssembler::assembleFrame(uint8_t msgIndex, uint8_t cborIndex)
{
	static uint8_t imageId = 0;
    LOG_TRACE("CAMERA_IMAGE frame: ", std::to_string(msgIndex), " completed with ", std::to_string(cborIndex), " cbor msgs");
    const auto storedMsg = m_cameraFramesQueue->getStoredMsg();
    std::vector<uint8_t> cborFrame;
    if(cborIndex != storedMsg)
    {
        LOG_ERROR("CAMERA_IMAGE frame: ", std::to_string(msgIndex), " completed with ", std::to_string(cborIndex), " cbor msgs, but ", std::to_string(storedMsg), " stored msgs in queue");
    }

    cborFrame.reserve(storedMsg * hardware_abstraction::Controllers::CAN_DATA_PAYLOAD_SIZE);

    //TODO check if iterate upto cborIndex or storedMsgs
    for(size_t idx = 0; idx < storedMsg; idx++)
    {
        std::array<uint8_t, hardware_abstraction::Controllers::CAN_DATA_PAYLOAD_SIZE> rxBuffer;
        m_cameraFramesQueue->receive(rxBuffer.data());
        cborFrame.insert(cborFrame.end(), rxBuffer.begin() + 2, rxBuffer.end());
    }
    const auto pendingStoredMsg = m_cameraFramesQueue->getStoredMsg();
    if(pendingStoredMsg != 0)
        LOG_WARNING("CAMERA_IMAGE frame: pending frames after read queue: ", std::to_string(pendingStoredMsg));

    auto it = std::find_if(cborFrame.rbegin(), cborFrame.rend(), [](uint8_t val) { return val != 0xFF; });
    cborFrame.erase(it.base(), cborFrame.end());

    try
    {
        auto m_dataSerializer = std::make_shared<business_logic::DataSerializer>();
        auto cborImgChunk = std::make_shared<business_logic::ImageSnapshot>();
        m_dataSerializer->deserialize(*cborImgChunk, cborFrame);

        static uint8_t totalChunks = 0;
		if(imageId != cborImgChunk->m_msgId)
		{
			assembleImage(imageId, totalChunks);
		}
		imageId = cborImgChunk->m_msgId;
		totalChunks = cborImgChunk->m_msgIndex;

        //TODO store the CBOR_CHUNK in the cborChunkQueue
		m_cameraSnapshotsQueue->sendToBack(( void * ) cborImgChunk.get());
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Exception caught: " + std::string(e.what()));
    }
    catch (...)
    {
        LOG_ERROR("Unknown exception caught during frame deserialization.");
    }
    return true;
}
}
}
