#include "Coordinate3DPoint.h"


Coordinate3DPoint::Coordinate3DPoint(float xCoord, float yCoord, float zCoord) : x(xCoord), y(yCoord), z(zCoord) 
{

}

Coordinate3DPointHomogeneous::Coordinate3DPointHomogeneous(const Coordinate3DPoint& point)
    : x(point.x), y(point.y), z(point.z), w(1.0f) 
{

}  

std::ostream& operator<<(std::ostream& os, const Coordinate3DPoint& point)
{
    os << point.x << ", " << point.y << ", " << point.z;
    return os;
}