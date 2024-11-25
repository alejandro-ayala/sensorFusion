#pragma once

namespace business_logic
{
namespace Communication
{
enum class CAN_IDs
{
	CLOCK_SYNC = 0x17,
	SENSOR_DATA = 0x18,
	LIDAR_3D_IMAGE
};
}
}
