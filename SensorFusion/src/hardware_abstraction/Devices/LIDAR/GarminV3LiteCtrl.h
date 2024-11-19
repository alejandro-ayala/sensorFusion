#pragma once

#include <Devices/LIDAR/LidarConfiguration.h>
#include "Controllers/I2C/I2CController.h"
#include <memory>

namespace hardware_abstraction
{
namespace Devices
{
class GarminV3LiteCtrl
{
public:
	GarminV3LiteCtrl(std::unique_ptr<Controllers::I2CController>&& i2cCtrl , const LidarConfiguration& cfg);
	virtual ~GarminV3LiteCtrl() = default;

    void initialization();
    void configuration(GarminV3LiteMode mode);
    uint16_t readDistance();
    bool applyBiasCorrection();
	bool selfTest();

private:
	bool enableTestMode();
	bool runTestMode();
	std::unique_ptr<Controllers::I2CController> m_i2cControl;
	GarminV3LiteMode m_mode;
	uint8_t m_addr;
	bool m_initialized;
};
}
}
