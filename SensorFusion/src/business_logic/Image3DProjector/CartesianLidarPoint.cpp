#include "CartesianLidarPoint.h"


CartesianLidarPoint::CartesianLidarPoint(uint16_t distance, uint8_t angleH, uint8_t angleV)
{

    xCoord = distance * std::sin(angleV * PI / 180) * std::cos(angleH * PI / 180);
    yCoord = distance * std::sin(angleV) * std::sin(angleH * PI / 180);
    zCoord = distance * std::cos(angleV * PI / 180);
}

bool CartesianLidarPoint::operator==(const CartesianLidarPoint& other) const
{
    return  xCoord == other.xCoord &&
            yCoord == other.yCoord &&
            zCoord == other.zCoord;
}