#pragma once
#include "Definitions.h"
#include <stdint.h>
#include <array>
namespace business_logic
{
struct CartesianLidarPoint
{
	CartesianLidarPoint() = default;
	CartesianLidarPoint(uint16_t distance, uint8_t angleH, uint8_t angleV);
    bool operator==(const CartesianLidarPoint& other) const;
	float xCoord;
	float yCoord;
	float zCoord;
};
using CartesianLidarArray = std::array<business_logic::CartesianLidarPoint, IMAGE3D_SIZE>;
}
