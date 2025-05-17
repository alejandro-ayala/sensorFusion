#pragma once 
#include <iostream>
#include <string>

struct Coordinate3DPoint
{
	Coordinate3DPoint() = default;
	Coordinate3DPoint(float xCoord, float yCoord, float zCoord);
	float x;
	float y;
	float z;
};

struct Coordinate3DPointHomogeneous
{
    Coordinate3DPointHomogeneous(const Coordinate3DPoint& point);
	float x;
	float y;
	float z;
    float w;
};

std::ostream& operator<<(std::ostream& os, const Coordinate3DPoint& point);
