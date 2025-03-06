#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "xparameters.h"
#include "netif/xadapter.h"
#include <platform_config.h>
#include <business_logic/Conectivity/ConnectionSettings.h>
#include <business_logic/Conectivity/CryptoMng.h>
#include <business_logic/Conectivity/HTTPConnectionTypes.h>
#include <business_logic/Image3DProjector/Image3DProjector.h>

#include "FreeRTOS.h"
#include "task.h"
#include "../external/mbedtls/include/mbedtls/platform.h"
#include <business_logic/Conectivity/ServerManager.h>
#include <business_logic/Communication/CommunicationManager.h>
#include <hardware_abstraction/Controllers/CAN/CanController.h>

#include "lwip/dhcp.h"
#include "lwip/init.h"
#include "lwip/sockets.h"
#include "lwipopts.h"

#include <business_logic/ClockSyncronization/TimeController.h>
#include <business_logic/ClockSyncronization/TimeBaseManager.h>
#include <business_logic/Conectivity/HTTPClient.h>
#include "application/SystemTasksManager/SystemTasksManager.h"
#include "services/Logger/LoggerMacros.h"

using namespace hardware_abstraction::Controllers;
using namespace hardware_abstraction::Devices;
using namespace business_logic::Conectivity;
using namespace business_logic::Communication;
using namespace business_logic::ClockSyncronization;
using namespace business_logic;



static std::unique_ptr<ImageCapturer3D> image3dCapturer;
static std::shared_ptr<CommunicationManager> commMng;
static std::shared_ptr<TimeBaseManager> globalClkMng;
static std::shared_ptr<HTTPClient> httpClient;
static std::unique_ptr<application::SystemTasksManager> systemTaskHandler;

static std::shared_ptr<business_logic::ClockSyncronization::TimeController> timecontroller;
static std::shared_ptr<CanController> canController;
application::TaskParams systemTaskMngParams;

void createHardwareAbstractionLayerComponents()
{
	canController = std::make_shared<CanController>();
	LOG_INFO("Created Hardware Abstraction layer components");
}

void createBusinessLogicLayerComponents()
{
	timecontroller = std::make_shared<TimeController>();
	commMng = std::make_shared<CommunicationManager>(timecontroller, canController);
	//httpClient = std::make_shared<HTTPClient>();
	globalClkMng = std::make_shared<TimeBaseManager>(timecontroller, canController);//, httpClient);
	LOG_INFO("Created Business Logic layer components");
}

void createApplicationLayerComponents()
{
	systemTaskMngParams.globalClkMng    = globalClkMng;
	systemTaskMngParams.commMng         = commMng;
	systemTaskHandler = std::make_unique<application::SystemTasksManager>(std::move(systemTaskMngParams));
	systemTaskHandler->createPoolTasks();
	LOG_DEBUG("Created Application layer components");
}

int main()
{
/************************
	std::cout << "Starting the program" << std::endl;

	const std::vector<std::vector<float>> lidarData;// = ImageReader().readLidarData();

	auto image3dProjector = Image3DProjector();
	const auto projectedPoints = image3dProjector.project3DImageTo2D(lidarData);

	uint16_t width = 1240, height = 380;
	//ImageWriter(width, height, "output.bmp").createBMPImage(projectedPoints);
	std::cout << "createBMPImage done" << std::endl;


	std::vector<std::vector<float>> projectedImg;
	for(const auto& point : lidarData)
	{

		Coordinate3DPoint lidarPoint(point[0], point[1], point[2]);
		const auto projectedPoint = image3dProjector.project3DPointTo2D(lidarPoint);
		std::vector<float> projectedPointVector{projectedPoint.x, projectedPoint.y, projectedPoint.z};
		projectedImg.push_back(projectedPointVector);
	}
	//ImageWriter(width, height, "output2.bmp").createBMPImage(projectedPoints);
    return 0;

************************/


	LOG_INFO("*********************Starting SensorFusion Node Zybo Z7*********************");
	createHardwareAbstractionLayerComponents();
	createBusinessLogicLayerComponents();
	createApplicationLayerComponents();

	vTaskStartScheduler();
	while(1);
	return 0;
}
