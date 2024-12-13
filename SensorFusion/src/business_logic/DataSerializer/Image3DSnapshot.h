#pragma once

#include "ISerializableMessage.h"
#include <iostream>
#include "business_logic/ImageCapturer3D/CartesianLidarPoint.h"
#include "business_logic/ImageCapturer3D/ImageCapturer3DConfig.h"
#include "services/Exception/SystemExceptions.h"
#include "json/include/nlohmann/json.hpp"

namespace business_logic
{

inline void to_json(nlohmann::json& j, const CartesianLidarPoint& point)
{
	    j = nlohmann::json{
	        {"xCoord", point.xCoord},
	        {"yCoord", point.yCoord},
	        {"zCoord", point.zCoord}
	    };
}

inline void from_json(const nlohmann::json& j, CartesianLidarPoint& point)
{
	    j.at("xCoord").get_to(point.xCoord);
	    j.at("yCoord").get_to(point.yCoord);
	    j.at("zCoord").get_to(point.zCoord);
}

class Image3DSnapshot : public ISerializableMessage
{
public:
	Image3DSnapshot() = default;
	Image3DSnapshot(uint8_t msgId, uint8_t m_msgIndex, const std::shared_ptr<std::array<CartesianLidarPoint, IMAGE3D_SIZE>> image3d, uint16_t image3dSize, uint64_t timestamp, uint64_t captureDeltaTime);
	~Image3DSnapshot() = default;

    void serialize(std::vector<uint8_t>& serializedData) const override;
    void deserialize(const std::vector<uint8_t>& data) override;
    inline bool operator==(const Image3DSnapshot& other)
	{
    	return (m_msgId       == other.m_msgId &&
    			m_msgIndex    == other.m_msgIndex &&
				m_timestamp   == other.m_timestamp &&
				m_captureDeltaTime   == other.m_captureDeltaTime &&
				m_image3dSize == other.m_image3dSize &&
				m_image3d     == other.m_image3d);
	}
//private:
public:
    uint8_t  m_msgId;
    uint8_t  m_msgIndex;
    std::shared_ptr<std::array<CartesianLidarPoint, IMAGE3D_SIZE>> m_image3d;
    uint16_t m_image3dSize;
    uint64_t m_timestamp;
    uint64_t m_captureDeltaTime;
    friend void to_json(nlohmann::json& j, const Image3DSnapshot& image)
    {
		try
		{
			j = nlohmann::json{{"msgId", image.m_msgId}, {"imgSize", image.m_image3dSize},
							   {"m_image3d", *image.m_image3d}, {"timestamp", image.m_timestamp}, {"captureDeltaTime", image.m_captureDeltaTime}};
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
        *image.m_image3d = j.template get<std::array<CartesianLidarPoint, IMAGE3D_SIZE>>();
    	image.m_timestamp = j.at("timestamp").get<uint64_t>();
    	image.m_captureDeltaTime = j.at("captureDeltaTime").get<uint64_t>();

    }
};
}
