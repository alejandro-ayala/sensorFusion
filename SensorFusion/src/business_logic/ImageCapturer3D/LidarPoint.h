#pragma once
#include <stdint.h>

namespace business_logic
{
struct LidarPoint
{
	LidarPoint() = default;
	LidarPoint(uint16_t distance, uint8_t angleH, uint8_t angleV);
    bool operator==(const LidarPoint& other) const;
	uint16_t m_pointDistance;
	uint8_t m_angleServoH;
	uint8_t m_angleServoV;
};
}
