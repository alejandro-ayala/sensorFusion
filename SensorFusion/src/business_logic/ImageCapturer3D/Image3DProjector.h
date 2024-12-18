#pragma once
#include <array>
#include <vector>
#include "CartesianLidarPoint.h"

namespace business_logic
{

struct Projected3DPoint
{
	Projected3DPoint() = default;
	Projected3DPoint(float a, float b, float c) : u(a), v(b), w(c) {};
	float u;
	float v;
	float w;
};

class Image3DProjector
{
private:
	const float m_focalLenX = 1;
	const float m_opticalCenterX = 1;
	const float m_focalLenY = 1;
	const float m_opticalCenterY = 1;
	const std::vector<std::vector<float>> m_cameraIntrinsicMatrix{{
	    {{m_focalLenX, 0.0, m_opticalCenterX}},
	    {{0.0, m_focalLenY, m_opticalCenterY}},
	    {{0.0, 0.0, 1.0}}
	}};
	const std::vector<std::vector<float>> m_cameraExtrinsicMatrix{{
	    {{0.0, 1.0, -1.0, 0.0}},
	    {{0.0, 1.0, 1.0, -10.0}},
	    {{1.0, 0.0, 0.0, 0.0}}
	}};
//	std::vector<std::vector<float>> multiplyMatrix(const std::vector<std::vector<float>>& a, const std::vector<std::vector<float>>& b) const;
	Projected3DPoint applyExtrinsicTransformation(const Projected3DPoint& lidarCoordPoint);
	Projected3DPoint applyIntrinsicProjection(const Projected3DPoint& cameraCoordPoint);
public:
	Image3DProjector() = default;
	~Image3DProjector() = default;
	Projected3DPoint project3DPointTo2D(const CartesianLidarPoint& lidarPoint);
	std::vector<std::vector<float>> multiplyMatrix(const std::vector<std::vector<float>>& a, const std::vector<std::vector<float>>& b) const;

};
}
