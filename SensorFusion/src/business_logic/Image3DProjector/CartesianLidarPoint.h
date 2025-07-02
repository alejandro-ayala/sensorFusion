#pragma once

#include <array>
#include <cstdint>
#include <cmath>

constexpr auto PI = 3.1416;//std::acos(-1);
static const uint16_t IMAGE3D_SIZE = 2048;

struct CartesianLidarPoint
{
	CartesianLidarPoint() = default;
	CartesianLidarPoint(uint16_t distance, uint8_t angleH, uint8_t angleV);
	bool operator==(const CartesianLidarPoint& other) const;

	float xCoord;
	float yCoord;
	float zCoord;
};
using CartesianLidarArray = std::array<CartesianLidarPoint, IMAGE3D_SIZE>;