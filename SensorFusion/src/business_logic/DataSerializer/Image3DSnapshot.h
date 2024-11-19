#pragma once

#include "ISerializableMessage.h"
#include <iostream>
#include "business_logic/ImageCapturer3D/LidarPoint.h"
#include "business_logic/ImageCapturer3D/ImageCapturer3DConfig.h"
#include "services/Exception/SystemExceptions.h"
#include "json/include/nlohmann/json.hpp"

namespace business_logic
{

inline void to_json(nlohmann::json& j, const LidarPoint& point)
{
	    j = nlohmann::json{
	        {"pointDistance", point.m_pointDistance},
	        {"angleServoH", point.m_angleServoH},
	        {"angleServoV", point.m_angleServoV}
	    };
}

inline void from_json(const nlohmann::json& j, LidarPoint& point)
{
	    j.at("pointDistance").get_to(point.m_pointDistance);
	    j.at("angleServoH").get_to(point.m_angleServoH);
	    j.at("angleServoV").get_to(point.m_angleServoV);
}

class Image3DSnapshot : public ISerializableMessage
{
public:
	Image3DSnapshot() = default;
	Image3DSnapshot(uint8_t msgId, uint8_t m_msgIndex, const std::array<LidarPoint, IMAGE3D_SIZE>& image3d, uint16_t image3dSize, uint32_t timestamp);
	~Image3DSnapshot() = default;

    void serialize(std::vector<uint8_t>& serializedData) const override;
    void deserialize(const std::vector<uint8_t>& data) override;
    inline bool operator==(const Image3DSnapshot& other)
	{
    	return (m_msgId       == other.m_msgId &&
    			m_msgIndex    == other.m_msgIndex &&
				m_timestamp   == other.m_timestamp &&
				m_image3dSize == other.m_image3dSize &&
				m_image3d     == other.m_image3d);
	}
private:
    uint8_t  m_msgId;
    uint8_t  m_msgIndex;
    std::array<LidarPoint, IMAGE3D_SIZE> m_image3d;
    uint16_t m_image3dSize;
    uint32_t m_timestamp;

    friend void to_json(nlohmann::json& j, const Image3DSnapshot& image)
    {
		try
		{
			j = nlohmann::json{{"msgId", image.m_msgId}, {"imgSize", image.m_image3dSize},
							   {"m_image3d", image.m_image3d}, {"timestamp", image.m_timestamp}};
		}
		catch (const std::exception& e)
		{
			THROW_BUSINESS_LOGIC_EXCEPTION(services::BusinessLogicErrorId::SerializationError, "Error during Image3DSnapshot to json serialization");
		}
    }

    friend void from_json(const nlohmann::json& j, Image3DSnapshot& image)
    {
    	image.m_msgId = j.at("msgId").get<uint8_t>();
    	image.m_image3dSize = j.at("imgSize").get<size_t>();
        image.m_image3d = j.template get<std::array<LidarPoint, IMAGE3D_SIZE>>();
    	image.m_timestamp = j.at("timestamp").get<uint32_t>();
    }
};
}
