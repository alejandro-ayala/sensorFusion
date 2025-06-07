#include "ImageSnapshot.h"
#include "services/Exception/SystemExceptions.h"
#include "services/Logger/LoggerMacros.h"
#include <vector>

namespace business_logic
{

ImageSnapshot::ImageSnapshot(uint8_t msgId, uint8_t msgIndex, uint8_t* msgBuffer, uint32_t imgSize, uint64_t timestamp) : m_msgId(msgId), m_msgIndex(msgIndex), m_imgSize(imgSize), m_timestamp(timestamp), m_imgBuffer(std::make_unique<uint8_t[]>(imgSize))
{
	BUSINESS_LOGIC_ASSERT( m_imgBuffer != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "Can not allocate memory for image snapshot msg");
	//std::copy(msgBuffer + imgSize*msgIndex, (msgBuffer + imgSize*msgIndex) + imgSize, this->m_imgBuffer);
	std::copy(msgBuffer, msgBuffer + imgSize, m_imgBuffer.get());
}

ImageSnapshot& ImageSnapshot::operator=(ImageSnapshot&& other) noexcept
{
    if (this != &other) {
        m_msgId = other.m_msgId;
        m_msgIndex = other.m_msgIndex;
        m_imgSize = other.m_imgSize;
        m_timestamp = other.m_timestamp;
        m_imgBuffer = std::move(other.m_imgBuffer);  // Mover el puntero único
    }
    return *this;
}

void ImageSnapshot::serialize(std::vector<uint8_t>& serializeData) const
{
    try
    {
        nlohmann::json j = *this;
        serializeData = nlohmann::json::to_cbor(j);
    }
    catch (const std::exception& e)
    {

    }
}

void ImageSnapshot::deserialize(const std::vector<uint8_t>& serializeMsg)
{
    nlohmann::json deserialized_json = nlohmann::json::from_cbor(serializeMsg);
    // Deserializa los valores manualmente en lugar de usar el operador de asignación
    m_msgId = deserialized_json["msgId"];
    m_msgIndex = deserialized_json["msgIndex"];
    m_imgSize = deserialized_json["imgSize"];
    m_timestamp = deserialized_json["timestamp"];

    m_imgBuffer = std::make_unique<uint8_t[]>(m_imgSize);
    std::memcpy(m_imgBuffer.get(), deserialized_json["imgBuffer"].get<std::vector<uint8_t>>().data(), m_imgSize);
//	std::string imgChunkBuffer = "imgChunkBuffer: ";
//	for(int i = 0; i< 128; i++)
//		imgChunkBuffer += std::to_string(m_imgBuffer[i]) + " ";
//	LOG_INFO(imgChunkBuffer);
}
}
