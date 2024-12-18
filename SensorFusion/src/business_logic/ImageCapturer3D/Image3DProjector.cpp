#include "Image3DProjector.h"
#include "services/Exception/SystemExceptions.h"
#include "services/Logger/LoggerMacros.h"

namespace business_logic
{

std::vector<std::vector<float>> Image3DProjector::multiplyMatrix(const std::vector<std::vector<float>>& a, const std::vector<std::vector<float>>& b) const
{
	LOG_DEBUG("Multiply matrix: ", std::to_string(a.at(0).size()), "x", std::to_string(b.size()));
	BUSINESS_LOGIC_ASSERT((a.at(0).size() == b.size()), services::BusinessLogicErrorId::MatrixDimensionError, "Wrong matrix dimension. Can not be multiplied");

	std::vector<std::vector<float>> result(a.size(), std::vector<float>(a.at(0).size()));

	for(size_t rowIdx = 0; rowIdx < a.size(); rowIdx++)
	{
		for(size_t columnIdx = 0; columnIdx < b.at(0).size(); columnIdx++)
		{
			for(size_t idx = 0; idx < b.size(); idx++)
			{
				result[rowIdx][columnIdx] += a[rowIdx][idx] * b[idx][columnIdx];
			}
		}
	}
	LOG_DEBUG("Result matrix size: ", std::to_string(result.size()), "x", std::to_string(result.at(0).size()));
	return result;
}

Projected3DPoint Image3DProjector::applyExtrinsicTransformation(const Projected3DPoint& lidarPoint)
{
	std::vector<std::vector<float>>lidarCoordPoint{{{{lidarPoint.u}}, {{lidarPoint.v}}, {{lidarPoint.w}}, {{1}}}};
	const auto camCoorPoint = multiplyMatrix(m_cameraExtrinsicMatrix, lidarCoordPoint);
	return Projected3DPoint(camCoorPoint[0][0], camCoorPoint[1][0], camCoorPoint[2][0]);

}

Projected3DPoint Image3DProjector::applyIntrinsicProjection(const Projected3DPoint& camCoordPoint)
{
	std::vector<std::vector<float>>cameraCoordPoint{{{{camCoordPoint.u}}, {{camCoordPoint.v}}, {{camCoordPoint.w}}}};
	const auto projectedPoint = multiplyMatrix(m_cameraIntrinsicMatrix, cameraCoordPoint);
	return Projected3DPoint();
}

Projected3DPoint Image3DProjector::project3DPointTo2D(const CartesianLidarPoint& lidarPoint)
{
	LOG_DEBUG("Projecting 3D point: ", std::to_string(lidarPoint.xCoord), ",", std::to_string(lidarPoint.yCoord), ",", std::to_string(lidarPoint.zCoord));
	const auto distance = lidarPoint.zCoord;
	const auto transformedPoint = applyExtrinsicTransformation(Projected3DPoint(lidarPoint.xCoord, lidarPoint.yCoord, lidarPoint.zCoord));
	const auto projectedPoint   = applyIntrinsicProjection(transformedPoint);
	LOG_DEBUG("Projected 2D point: ", std::to_string(projectedPoint.u), ",", std::to_string(projectedPoint.v), " for distance: ", std::to_string(distance));
	return Projected3DPoint(projectedPoint.u, projectedPoint.v, distance);

}
}
