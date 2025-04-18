#pragma once

#include "ISerializableMessage.h"
#include <iostream>
#include "services/Logger/LoggerMacros.h"

#define MAXIMUN_CBOR_BUFFER_SIZE 128
namespace business_logic
{
class ImageSnapshot : public ISerializableMessage
{
public:
	ImageSnapshot() = default;
	ImageSnapshot(uint8_t msgId, uint8_t m_msgIndex, uint8_t* msgBuffer, uint32_t msgSize, uint64_t timestamp);
	~ImageSnapshot() = default;
	ImageSnapshot(const ImageSnapshot&) = delete;  // Deshabilitar copia
	ImageSnapshot& operator=(const ImageSnapshot&) = delete;  // Deshabilitar asignación por copia
	ImageSnapshot& operator=(ImageSnapshot&& other) noexcept;
    void serialize(std::vector<uint8_t>& serializedData) const override;
    void deserialize(const std::vector<uint8_t>& data) override;
    inline bool operator==(const ImageSnapshot& other)
	{
    	return (m_msgId      == other.m_msgId &&
    			m_msgIndex   == other.m_msgIndex &&
    			m_imgSize    == other.m_imgSize &&
				m_timestamp  == other.m_timestamp &&
				std::memcmp(m_imgBuffer.get(), other.m_imgBuffer.get(), MAXIMUN_CBOR_BUFFER_SIZE) == 0);
	}
//private:
    uint8_t  m_msgId;
    uint8_t  m_msgIndex;
    std::unique_ptr<uint8_t[]> m_imgBuffer;
    uint32_t m_imgSize;
    uint64_t m_timestamp;
private:
    friend void to_json(nlohmann::json& j, const ImageSnapshot& image)
    {
        if (image.m_imgSize > 0 && image.m_imgBuffer != nullptr)
        {
            try
            {
                std::vector<uint8_t> imgBufferVector(image.m_imgBuffer.get(), image.m_imgBuffer.get() + image.m_imgSize);
                j = nlohmann::json{{"msgId", image.m_msgId}, {"msgIndex", image.m_msgIndex}, {"imgSize", image.m_imgSize},
                                   {"imgBuffer", imgBufferVector}, {"timestamp", image.m_timestamp}};
            } catch (const std::exception& e) {
                std::cerr << "Error al asignar memoria al vector: " << e.what() << std::endl;
            }
        }
        else
        {
            std::cerr << "Error: m_imgSize es 0 o m_imgBuffer es un puntero nulo." << std::endl;
        }
    }

    friend void from_json(const nlohmann::json& j, ImageSnapshot& image)
    {
    	image.m_msgId = j.at("msgId").get<uint8_t>();
    	image.m_msgIndex = j.at("msgIndex").get<uint8_t>();
    	image.m_imgSize = j.at("imgSize").get<size_t>();
        std::vector<uint8_t> imgBufferVector = j.at("imgBuffer").get<std::vector<uint8_t>>();
        LOG_INFO("ImageSnapshot::from_json() --> ", std::to_string(image.m_imgSize));
        image.m_imgBuffer = std::make_unique<uint8_t[]>(imgBufferVector.size());
        std::copy(imgBufferVector.begin(), imgBufferVector.end(), image.m_imgBuffer.get());
    	image.m_timestamp = j.at("timestamp").get<uint64_t>();
    }
};
}
