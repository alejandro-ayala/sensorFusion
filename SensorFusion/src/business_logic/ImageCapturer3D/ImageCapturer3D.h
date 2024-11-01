#pragma once

#include "ImageCapturer3DConfig.h"
#include "business_logic/Osal/QueueHandler.h"
#include <array>
#include <memory>
static const uint8_t IMAGE3D_SIZE = 128;
static const uint8_t IMAGE3D_SAMPLES_PER_POSITION = 5;

namespace business_logic
{
class ImageCapturer3D
{
private:
	std::shared_ptr<hardware_abstraction::Devices::ServoMotorControl> m_horServoCtrl;
	std::shared_ptr<hardware_abstraction::Devices::ServoMotorControl> m_verServoCtrl;
	std::shared_ptr<hardware_abstraction::Devices::GarminV3LiteCtrl>  m_lidarCtrl;
	ImageCapturer3DConfig m_config;
	std::array<uint16_t, IMAGE3D_SIZE> m_3DImage;
	bool m_applyBiasCorrection = false;

	void sortDistanceImage(std::array<uint16_t, IMAGE3D_SAMPLES_PER_POSITION>& distancePointSamples);
	uint16_t getDistanceMedian(std::array<uint16_t, IMAGE3D_SAMPLES_PER_POSITION>& distancePointSamples) const;
	uint16_t getPointDistance() const;


public:
	explicit ImageCapturer3D(const ImageCapturer3DConfig& config);
	virtual ~ImageCapturer3D() = default;

    void initialize();
    void stop();
    void captureImage();
};
}
