#pragma once 
#include "Image3DProjectorConfig.h"
#include "Coordinate3DPoint.h"
#include "CartesianLidarPoint.h"

class Image3DProjector
{
public:
	Image3DProjectorConfig m_projectionConfig;

//	std::vector<std::vector<float>> multiplyMatrix(const std::vector<std::vector<float>>& a, const std::vector<std::vector<float>>& b) const;
	Coordinate3DPoint applyExtrinsicTransformation(const Coordinate3DPoint& lidarCoordPoint);
	Coordinate3DPoint applyIntrinsicProjection(const Coordinate3DPoint& cameraCoordPoint);
	std::vector<std::vector<float>> project3DImageTo2D(const std::vector<std::vector<float>>& image3d);
public:
	Image3DProjector(const Image3DProjectorConfig& config = Image3DProjectorConfig()) : m_projectionConfig(config) {};
	~Image3DProjector() = default;
	Coordinate3DPoint project3DPointTo2D(const Coordinate3DPoint& lidarPoint);
	std::vector<std::vector<float>> multiplyMatrix(const std::vector<std::vector<float>>& a, const std::vector<std::vector<float>>& b) const;
};