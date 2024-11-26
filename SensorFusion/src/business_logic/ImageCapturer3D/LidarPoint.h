#pragma once
#include <stdint.h>
#include <array>
namespace business_logic
{
static const uint16_t IMAGE3D_SIZE = 2048;
struct LidarPoint
{
	LidarPoint() = default;
	LidarPoint(uint16_t distance, uint8_t angleH, uint8_t angleV);
    bool operator==(const LidarPoint& other) const;
	uint16_t m_pointDistance;
	uint8_t m_angleServoH;
	uint8_t m_angleServoV;
};
using LidarArray = std::array<business_logic::LidarPoint, IMAGE3D_SIZE>;
}
