#include "Image3DProjector.h"

std::vector<std::vector<float>> Image3DProjector::multiplyMatrix(const std::vector<std::vector<float>>& a, const std::vector<std::vector<float>>& b) const
{
	//std::cout << "Multiply matrix: A: " << std::to_string(a.size())  << "x" << std::to_string(a.at(0).size()) << " and B:" << std::to_string(b.size()) << "x" << std::to_string(b.at(0).size()) << std::endl;
	//BUSINESS_LOGIC_ASSERT((a.at(0).size() == b.size()), services::BusinessLogicErrorId::MatrixDimensionError, "Wrong matrix dimension. Can not be multiplied");
	std::vector<std::vector<float>> result(a.size(), std::vector<float>(a.at(0).size()));
	//std::cout << "a.size
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
	//LOG_DEBUG("Result matrix size: ", std::to_string(result.size()), "x", std::to_string(result.at(0).size()));
	return result;
}

Coordinate3DPoint Image3DProjector::applyExtrinsicTransformation(const Coordinate3DPoint& lidarPoint)
{
	Coordinate3DPointHomogeneous lidarPointHomog(lidarPoint);
	std::vector<std::vector<float>>lidarCoordPoint{{{{lidarPointHomog.x}}, {{lidarPointHomog.y}}, {{lidarPointHomog.z}}, {{lidarPointHomog.w}}}};
	const auto camCoorPoint = multiplyMatrix(m_projectionConfig.m_cameraExtrinsicMatrix, lidarCoordPoint);
	return Coordinate3DPoint(camCoorPoint[0][0], camCoorPoint[1][0], camCoorPoint[2][0]);
}

Coordinate3DPoint Image3DProjector::applyIntrinsicProjection(const Coordinate3DPoint& camCoordPoint)
{
	std::vector<std::vector<float>>cameraCoordPoint{{{{camCoordPoint.x}}, {{camCoordPoint.y}}, {{camCoordPoint.z}}}};
	const auto projectedPoint = multiplyMatrix(m_projectionConfig.m_cameraIntrinsicMatrix, cameraCoordPoint);
	//std::cout << "projectedPoint: " << projectedPoint[0][0] << "," << projectedPoint[1][0]  << "," << projectedPoint[2][0] << std::endl;
	return Coordinate3DPoint(projectedPoint[0][0], projectedPoint[1][0], projectedPoint[2][0]);
}

Coordinate3DPoint Image3DProjector::project3DPointTo2D(const Coordinate3DPoint& lidarPoint)
{
	//LOG_DEBUG("Projecting 3D point: ", std::to_string(lidarPoint.xCoord), ",", std::to_string(lidarPoint.yCoord), ",", std::to_string(lidarPoint.zCoord));
	//std::cout << "Projecting 3D point: " <<  lidarPoint  << " to 2D plane" << std::endl;
	const auto transformedPoint = applyExtrinsicTransformation(lidarPoint);
	//std::cout << "transformedPoint: " << transformedPoint.x << "," << transformedPoint.y  << "," << transformedPoint.z << std::endl;
	const auto projectedPoint   = applyIntrinsicProjection(transformedPoint);
	//std::cout << "Projected 3D point: " <<  projectedPoint  << " to 2D plane" << std::endl;

	return Coordinate3DPoint(projectedPoint.x / projectedPoint.z, projectedPoint.y / projectedPoint.z, projectedPoint.z);
}

std::vector<std::vector<float>> Image3DProjector::project3DImageTo2D(const std::vector<std::vector<float>>& image3d)
{
	auto t_projection = multiplyMatrix(m_projectionConfig.m_cameraIntrinsicMatrix, m_projectionConfig.m_cameraExtrinsicMatrix);	
	std::vector<std::vector<float>> projectedPoints;

	try
	{
		std::cout << "Projecting lidar data to camera plane" << std::endl;
		for (auto sample : image3d)
		{

			const std::vector<std::vector<float>> sampleLidarPoint{{{{sample[0]}}, {{sample[1]}}, {{sample[2]}}, {{1.0}} }};
			auto projectedPoint = multiplyMatrix(t_projection, sampleLidarPoint);
			projectedPoints.push_back(std::vector<float>{projectedPoint[0][0] / projectedPoint[2][0], projectedPoint[1][0] / projectedPoint[2][0], projectedPoint[2][0]});
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	return projectedPoints;
}