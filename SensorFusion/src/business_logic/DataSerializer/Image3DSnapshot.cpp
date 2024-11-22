#include <business_logic/DataSerializer/Image3DSnapshot.h>
#include "services/Exception/SystemExceptions.h"

#include <vector>

namespace business_logic
{

Image3DSnapshot::Image3DSnapshot(uint8_t msgId, uint8_t msgIndex, const std::shared_ptr<std::array<LidarPoint, IMAGE3D_SIZE>> image3d, uint16_t image3dSize, uint32_t timestamp) : m_msgId(msgId), m_msgIndex(msgIndex), m_image3d(image3d), m_image3dSize(image3dSize), m_timestamp(timestamp)
{

}


void Image3DSnapshot::serialize(std::vector<uint8_t>& serializeData) const
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

void Image3DSnapshot::deserialize(const std::vector<uint8_t>& serializeMsg)
{
    nlohmann::json deserialized_json = nlohmann::json::from_cbor(serializeMsg);
    *this = deserialized_json.get<Image3DSnapshot>();
}
}

