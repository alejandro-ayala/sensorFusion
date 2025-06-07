#include "ImageCapturer3D.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include "services/Logger/LoggerMacros.h"
#include "services/Exception/SystemExceptions.h"
#define FAKE_VALUES
namespace business_logic
{
#include "xtime_l.h"
void preciseDelayUs(uint64_t delay_us) {
    XTime tStart, tEnd;
    XTime_GetTime(&tStart);
    do {
        XTime_GetTime(&tEnd);
    } while ((tEnd - tStart) < delay_us * (COUNTS_PER_SECOND / 1000000));
}

ImageCapturer3D::ImageCapturer3D(const ImageCapturer3DConfig& config):  m_horServoCtrl(config.horServoCtrl), m_verServoCtrl(config.verServoCtrl), m_lidarCtrl(config.lidarCtrl), m_config(config), m_initialized(false)
{

}

void ImageCapturer3D::initialize()
{
	m_horServoCtrl->initialize();
	m_verServoCtrl->initialize();

#ifndef FAKE_VALUES
	bool lidarInitialized = false;
	while(lidarInitialized == false)
	{

		try {
			m_lidarCtrl->initialization();
			lidarInitialized = true;
		}
		catch (const services::ControllersException& ex) {
			LOG_ERROR("ControllersException caught: ", ex.what());
			LOG_ERROR("Error ID: : ", static_cast<int>(ex.getErrorId()));
		}
		catch (const std::exception& ex) {
		    std::cerr << "Standard exception caught: " << ex.what() << std::endl;
		}
	}
#endif
}


void ImageCapturer3D::stop()
{

}

uint32_t ImageCapturer3D::captureImage()
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

//	uint8_t hor = 1, ver = 1, angleH = m_config.maxHorizontalAngle, angleV = m_config.minVerticalAngle;
//	while(1)
//	{
////		m_verServoCtrl->setAngle(angleV);
////		m_horServoCtrl->setAngle(angleH);
//		if(hor == 1)
//		{
//			hor = 0;
//			m_horServoCtrl->setAngle(angleH);
//
//		}
//		if(ver == 1)
//		{
//			ver = 0;
//			m_verServoCtrl->setAngle(angleV);
//		}
//		const auto pointDistance = getPointDistance();
//		auto point3d = LidarPoint(pointDistance,  angleH - m_config.offsetHorizontalAngle, angleV - m_config.offsetVerticalAngle);
//		std::string sample3D = std::to_string(point3d.m_angleServoH) + "," + std::to_string(point3d.m_angleServoV) + "," + std::to_string(point3d.m_pointDistance);
//		LOG_INFO(sample3D);
//	}
	m_horServoCtrl->setAngle(m_config.maxHorizontalAngle);
	//vTaskDelay(pdMS_TO_TICKS(m_config.settlingTime));
	preciseDelayUs(10000);
	m_verServoCtrl->setAngle(m_config.minVerticalAngle);
	//vTaskDelay(pdMS_TO_TICKS(m_config.settlingTime));
	preciseDelayUs(10000);

	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	for(int vAngle = m_config.minVerticalAngle; vAngle <= m_config.maxVerticalAngle; )
	{
		static int a = 0;
		a++;
		for(int hAngle = m_config.maxHorizontalAngle; hAngle >= m_config.minHorizontalAngle; hAngle -= m_config.horizontalAngleResolution)
		{
			m_horServoCtrl->setAngle(hAngle);
			const auto lidarPoint = getPointDistance();
			m_3DImage[image3dSize] = LidarPoint(lidarPoint,  hAngle - m_config.offsetHorizontalAngle, vAngle - m_config.offsetVerticalAngle);
			image3dSize++;
			//std::string distance = std::to_string(hAngle) + ", " + std::to_string(vAngle) + std::to_string(lidarPoint);
			//LOG_INFO(distance);
			//vTaskDelay(pdMS_TO_TICKS(m_config.settlingTime));
			preciseDelayUs(10000);
		}

		vAngle += m_config.verticalAngleResolution;
		m_verServoCtrl->setAngle(vAngle);
		vTaskDelay(pdMS_TO_TICKS(m_config.settlingTime));
		preciseDelayUs(10000);

		for(int hAngle = m_config.minHorizontalAngle; hAngle <= m_config.maxHorizontalAngle; hAngle += m_config.horizontalAngleResolution)
		{
			m_horServoCtrl->setAngle(hAngle);
			const auto lidarPoint = getPointDistance();
			m_3DImage[image3dSize] = LidarPoint(lidarPoint, hAngle - m_config.offsetHorizontalAngle, vAngle - m_config.offsetVerticalAngle);
			image3dSize++;
//			std::string distance = "Point distance (" + std::to_string(hAngle) + ", " + std::to_string(vAngle) + ") = " + std::to_string(lidarPoint);
//			LOG_DEBUG(distance);
//			std::string distance = std::to_string(hAngle) + ", " + std::to_string(vAngle) + std::to_string(lidarPoint);
//			LOG_INFO(distance);
			//vTaskDelay(pdMS_TO_TICKS(m_config.settlingTime));
			preciseDelayUs(10000);
		}

		vAngle += m_config.verticalAngleResolution;
		m_verServoCtrl->setAngle(vAngle);
		//vTaskDelay(pdMS_TO_TICKS(m_config.settlingTime));
		preciseDelayUs(10000);
	}
	return image3dSize;
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

LidarArray ImageCapturer3D::getLastCapture()
{
	return m_3DImage;
}

}
