#pragma once

#include "LidarConfiguration.h"
#include "Controllers/I2C/I2CController.h"
#include <memory>

namespace Devices
{
class GarminV3LiteCtrl
{
public:
	GarminV3LiteCtrl(const std::shared_ptr<Controllers::I2CController>& i2cCtrl , const LidarConfiguration& cfg);
	virtual ~GarminV3LiteCtrl() = default;

    void initialization();
    void configuration(GarminV3LiteMode mode);
    uint16_t readDistance();
	bool selfTest();

private:
	GarminV3LiteMode m_mode;
	uint8_t m_addr;
	std::shared_ptr<Controllers::I2CController> m_i2cControl;

};
}
