#pragma once

#include <business_logic/ImageCapturer3D/LidarPoint.h>
#include "ImageCapturer3DConfig.h"
#include "business_logic/Osal/QueueHandler.h"
#include <array>
#include <memory>

namespace business_logic
{
class ImageCapturer3D
{
private:
	std::shared_ptr<hardware_abstraction::Devices::ServoMotorControl> m_horServoCtrl;
	std::shared_ptr<hardware_abstraction::Devices::ServoMotorControl> m_verServoCtrl;
	std::shared_ptr<hardware_abstraction::Devices::GarminV3LiteCtrl>  m_lidarCtrl;
	ImageCapturer3DConfig m_config;
	std::array<LidarPoint, IMAGE3D_SIZE> m_3DImage;
	bool m_applyBiasCorrection = false;
	bool m_initialized;

	void sortDistanceImage(std::array<uint16_t, IMAGE3D_SAMPLES_PER_POSITION>& distancePointSamples);
	uint16_t getDistanceMedian(std::array<uint16_t, IMAGE3D_SAMPLES_PER_POSITION>& distancePointSamples) const;
	uint16_t getPointDistance() const;


public:
	explicit ImageCapturer3D(const ImageCapturer3DConfig& config);
	virtual ~ImageCapturer3D() = default;

    void initialize();
    void stop();
    void captureImage();
    std::array<LidarPoint, IMAGE3D_SIZE>& getLastCapture();
};
}
