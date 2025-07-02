#pragma once 

#include <vector>
struct Image3DProjectorConfig
{
	const float m_focalLenX = 718.856;
	const float m_opticalCenterX = 607.1928;
	const float m_focalLenY = 718.856;
	const float m_opticalCenterY = 185.2157;
	const std::vector<std::vector<float>> m_cameraIntrinsicMatrix{{
	    {{m_focalLenX, 0.0, m_opticalCenterX, 45.38225}},
	    {{0.0, m_focalLenY, m_opticalCenterY, -0.1130887}},
	    {{0.0, 0.0, 1.0, 0.003779761}}
	}};
	const std::vector<std::vector<float>> m_cameraExtrinsicMatrix{{
	    {{0.007967514, -0.9999679, -0.0008462264, -0.01377769}},
	    {{-0.002771053, 0.0008241710, -0.9999958, -0.05542117}},
	    {{0.9999644, 0.007969825, -0.002764397, -0.2918589}},
		{{0.0, 0.0, 0.0, 1.0}}
	}};
};