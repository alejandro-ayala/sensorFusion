#include "ImageAssembler.h"
#include "business_logic/DataSerializer/DataSerializer.h"
#include "business_logic/DataSerializer/ImageSnapshot.h"
#include "hardware_abstraction/Controllers/CAN/CanFrame.h"
#include "services/Logger/LoggerMacros.h"
#include "task.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

uint32_t cntLoadedImg = 0, cntFailedImg = 0;
namespace business_logic
{
namespace ImageClassifier
{
ImageAssembler::ImageAssembler(const std::shared_ptr<business_logic::Osal::QueueHandler>& cameraFramesQueue) : m_cameraFramesQueue(cameraFramesQueue)
{
	uint32_t queueItemSize   = sizeof(std::shared_ptr<business_logic::ImageSnapshot>);
	uint32_t queueLength     = 200;
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
	if((storedCborChunks-1) != totalChunks)
	{
		const std::string msg = "Queue contains " + std::to_string(storedCborChunks) + " and the image to assembly expects " + std::to_string(storedCborChunks) ;
		LOG_WARNING(msg);
	}
	else
	{
		const std::string cborImgChunkStr = "Completed ImageSnapshot: " + std::to_string(imageId) + " with " + std::to_string(totalChunks) + " chunks ";
		LOG_INFO(cborImgChunkStr);
	}

	bool discardImage = false;
	uint32_t assembledChunks = 0;
	for(uint32_t idx = 0; idx < storedCborChunks ; idx++)
	{
		bool insertFrame = true;
		//auto rxSnapshotChunk = std::make_shared<business_logic::ImageSnapshot>();
		std::shared_ptr<business_logic::ImageSnapshot> rxSnapshotChunk;
		m_cameraSnapshotsQueue->receive(rxSnapshotChunk);

        if(insertFrame && rxSnapshotChunk->m_msgId != imageId)
        {
        	const std::string msg = "Snapshot id: " + std::to_string(rxSnapshotChunk->m_msgId) + " different that " +std::to_string(imageId);
        	LOG_WARNING(msg);
        	insertFrame = false;
        }
        if(insertFrame && rxSnapshotChunk->m_msgIndex != idx)
        {
        	const std::string msg = "ChunkID: " + std::to_string(rxSnapshotChunk->m_msgIndex) + " not in sequence with previous " +std::to_string(idx);
        	LOG_WARNING(msg);
        	insertFrame = false;
        }
        if(insertFrame)
        {
        	assembledImage.insert(assembledImage.end(), rxSnapshotChunk->m_imgBuffer.get(), rxSnapshotChunk->m_imgBuffer.get() + rxSnapshotChunk->m_imgSize);
        	assembledChunks++;
        }
        else
        {
        	discardImage = true;
        	LOG_WARNING("Discarded frame: ", imageId);
        }
	}
	if(!discardImage)
	{
		static int i = 0;
		//TODO implement read chunks and assemble to provide to TensorFlowLite
		const auto assembledImageSize = assembledImage.size();
		auto assembledImagePtr = assembledImage.data();
		std::string assembledImageStr = "*********** Assembled Image " + std::to_string(imageId) + " with " + std::to_string(assembledChunks) + "/" + std::to_string(storedCborChunks) + " chunks and size: " + std::to_string(assembledImageSize) + " bytes --> "
				+ std::to_string(assembledImage[0]) + ", " + std::to_string(assembledImage[1]) + " -- " + std::to_string(assembledImage[assembledImage.size() - 2]) + ", " + std::to_string(assembledImage.size() - 1) + "***********";

		//		std::stringstream ss;
		//		for( const auto elem : assembledImage)
		//		{
		//		    ss << std::uppercase           // Letras en mayúsculas (A-F)
		//		       << std::setfill('0')        // Rellenar con ceros a la izquierda
		//		       << std::setw(2)             // Ancho fijo de 2 caracteres
		//		       << std::hex
		//		       << static_cast<int>(elem)   // Asegura que uint8_t no se imprima como char
		//		       << " ";
		//		}
		//		assembledImageStr += ss.str();
		LOG_TRACE(assembledImageStr);

		int width = 0, height = 0, channels = 0;
		if(stbi_info_from_memory(assembledImagePtr, static_cast<int>(assembledImageSize), &width, &height, &channels))
		{
			std::string loadImageStr = "Image can be load: " + std::to_string(width) + "x" + std::to_string(height) + " with " + std::to_string(channels) + " channels";
			LOG_INFO(loadImageStr);
			unsigned char* input_image = stbi_load_from_memory(assembledImagePtr, static_cast<int>(assembledImageSize), &width, &height, &channels, 1);
			if(input_image)
			{
				std::string loadImageStr = "***********Image loaded correctly(" + std::to_string(cntLoadedImg) + "/" + std::to_string(cntFailedImg) + "): " + std::to_string(width) + "x" + std::to_string(height) + " with " + std::to_string(channels) + " channels***********";
				LOG_INFO(loadImageStr);
				logMemoryUsage();
				stbi_image_free(input_image);
				logMemoryUsage();
				cntLoadedImg++;
			}
			else
			{
				logMemoryUsage();
				LOG_ERROR("Load from memory image FAILED!!!!!!!");
				cntFailedImg++;
			}
		}
		else
		{
			logMemoryUsage();
			LOG_ERROR("Image can not be Loaded from memory");
		}

		i++;
		const auto pendingCborChunks = m_cameraSnapshotsQueue->getStoredMsg();
		if(pendingCborChunks)
			LOG_WARNING("Pending CBOR chunks: ", pendingCborChunks);
	}
	return true;
}

bool ImageAssembler::assembleFrame(uint8_t msgIndex, uint8_t cborIndex, bool isEndOfImage)
{
	static uint8_t imageId = 0;
	static uint32_t testId = 0;
	const auto storedMsg = m_cameraFramesQueue->getStoredMsg();


    std::vector<uint8_t> cborFrame;
    if(cborIndex != storedMsg - 1)
    {
        LOG_ERROR("ImageAssembler::assembleFrame: ", std::to_string(msgIndex), " completed with ", std::to_string(cborIndex), " cbor msgs, but ", std::to_string(storedMsg), " stored msgs in queue");
    }
    else if((testId % 30) == 0)
	{
		LOG_TRACE("ImageAssembler::assembleFrame:  ", std::to_string(msgIndex), " completed with ", std::to_string(cborIndex), " cbor msgs,  and ", std::to_string(storedMsg), " stored msgs in queue");
	}
	testId++;

    cborFrame.reserve(storedMsg * hardware_abstraction::Controllers::CAN_DATA_PAYLOAD_SIZE);

    //TODO check if iterate upto cborIndex or storedMsgs
    for(size_t idx = 0; idx < storedMsg; idx++)
    {
        std::array<uint8_t, hardware_abstraction::Controllers::CAN_DATA_PAYLOAD_SIZE> rxBuffer;

        if (m_cameraFramesQueue->peek(rxBuffer.data()) == pdTRUE)
        {
        	if(rxBuffer[0] == msgIndex)
            {
                m_cameraFramesQueue->receive(rxBuffer.data());
                cborFrame.insert(cborFrame.end(), rxBuffer.begin() + 2, rxBuffer.end());
            }
            else
            {
            	LOG_WARNING("CAMERA_IMAGE frame from different CborChunk: ", std::to_string(rxBuffer[0]), " -- ", std::to_string(msgIndex) );
            }
        }

    }
    const auto pendingStoredMsg = m_cameraFramesQueue->getStoredMsg();
    if(pendingStoredMsg != 0)
        LOG_WARNING("CAMERA_IMAGE frame: pending frames after read queue: ", std::to_string(pendingStoredMsg));

    auto it = std::find_if(cborFrame.rbegin(), cborFrame.rend(), [](uint8_t val) { return val != 0xFF; });
    cborFrame.erase(it.base(), cborFrame.end());

    try
    {
        auto m_dataSerializer = std::make_shared<business_logic::DataSerializer>();
        business_logic::ImageSnapshot cborImgChunk;

        m_dataSerializer->deserialize(cborImgChunk, cborFrame);

        static uint8_t totalChunks = 0;
		imageId = cborImgChunk.m_msgId;
		totalChunks = cborImgChunk.m_msgIndex;

        //TODO store the CBOR_CHUNK in the cborChunkQueue
		auto snapshotPtr = std::make_shared<business_logic::ImageSnapshot>(cborImgChunk.m_msgId, cborImgChunk.m_msgIndex, cborImgChunk.m_imgBuffer.get(), cborImgChunk.m_imgSize, cborImgChunk.m_timestamp);
		m_cameraSnapshotsQueue->sendToBack(snapshotPtr);

		if(isEndOfImage)
		{
			assembleImage(imageId, totalChunks);
		}
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
