#include "LidarPoint.h"

namespace business_logic
{

LidarPoint::LidarPoint(uint16_t distance, uint8_t angleH, uint8_t angleV) : m_pointDistance(distance), m_angleServoH(angleH), m_angleServoV(angleV)
{

}

bool LidarPoint::operator==(const LidarPoint& other) const
{
    return m_pointDistance == other.m_pointDistance &&
    		m_angleServoH == other.m_angleServoH &&
			m_angleServoV == other.m_angleServoV;
}
}
