#include "ImageCapturer3D.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include "services/Logger/LoggerMacros.h"

//#define FAKE_VALUES
namespace business_logic
{
ImageCapturer3D::ImageCapturer3D(const ImageCapturer3DConfig& config):  m_horServoCtrl(config.horServoCtrl), m_verServoCtrl(config.verServoCtrl), m_lidarCtrl(config.lidarCtrl), m_config(config), m_initialized(false)
{

}

void ImageCapturer3D::initialize()
{
	m_horServoCtrl->initialize();
	m_verServoCtrl->initialize();
#ifndef FAKE_VALUES

	m_lidarCtrl->initialization();
#endif

}


void ImageCapturer3D::stop()
{

}

void ImageCapturer3D::captureImage()
{
	uint32_t image3dSize = 0;
	uint16_t delayServo = 1;
	if(m_config.applyBiasCorrection && m_applyBiasCorrection)
	{
		//Apply bias correction. Check when!!
		auto state = m_lidarCtrl->applyBiasCorrection();
		if(state)
		{
			//TODO handle error
			LOG_WARNING("Error applying bias correction");
		}
	}
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	for(int vAngle = m_config.initVerticalAngle; vAngle <= m_config.maxVerticalAngle; )
	{
		static int a = 0;
		a++;
		for(int hAngle = m_config.initHorizontalAngle; hAngle <= m_config.maxHorizontalAngle; hAngle += m_config.horizontalAngleResolution)
		{
			m_horServoCtrl->setAngle(hAngle);
			const auto lidarPoint = getPointDistance();
			m_3DImage[image3dSize] = LidarPoint(lidarPoint, hAngle, vAngle);
			image3dSize++;
		}

		vAngle += m_config.verticalAngleResolution;
		m_verServoCtrl->setAngle(vAngle);

		for(int hAngle = m_config.maxHorizontalAngle; hAngle >= m_config.initHorizontalAngle; hAngle -= m_config.horizontalAngleResolution)
		{
			m_horServoCtrl->setAngle(hAngle);
			const auto lidarPoint = getPointDistance();
			m_3DImage[image3dSize] = LidarPoint(lidarPoint, hAngle, vAngle);
			image3dSize++;
		}

		vAngle += m_config.verticalAngleResolution;
		m_verServoCtrl->setAngle(vAngle);
	}

}

uint16_t ImageCapturer3D::getPointDistance() const
{
#ifdef FAKE_VALUES
	for(int i=0;i<0xFFFFF;i++);
	return 0x3f;
#else
	std::array<uint16_t, IMAGE3D_SAMPLES_PER_POSITION> distance;
	for(auto idx = 0; idx < m_config.samplesPerPosition; idx++)
	{
		distance[idx] = m_lidarCtrl->readDistance();
	}
	auto pointDistance = getDistanceMedian(distance);
	return pointDistance;
#endif
}

void ImageCapturer3D::sortDistanceImage(std::array<uint16_t, IMAGE3D_SAMPLES_PER_POSITION>& distancePointSamples)
{
	return std::sort(distancePointSamples.begin(), distancePointSamples.end());
}

uint16_t ImageCapturer3D::getDistanceMedian(std::array<uint16_t, IMAGE3D_SAMPLES_PER_POSITION>& distancePointSamples) const
{
    size_t arraySize = distancePointSamples.size();
    uint16_t median;
    if (arraySize % 2 == 0)
    {
        median = (distancePointSamples[arraySize / 2 - 1] + distancePointSamples[arraySize / 2]) / 2.0;
    }
    else
    {
        median = distancePointSamples[arraySize / 2];
    }

    return median;

}

}
