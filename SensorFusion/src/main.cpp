
#include <hardware_abstraction/Devices/MotorControl/L298Hbridge.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "xparameters.h"
#include "netif/xadapter.h"
#include <platform_config.h>
#include "xil_printf.h"
#include <business_logic/Conectivity/ConnectionSettings.h>
#include <business_logic/Conectivity/CryptoMng.h>
#include <business_logic/Conectivity/HTTPConnectionTypes.h>

#include "FreeRTOS.h"
#include "task.h"
#include "../external/mbedtls/include/mbedtls/platform.h"
#include <business_logic/Conectivity/ServerManager.h>
#include <business_logic/Communication/CommunicationManager.h>
#include <hardware_abstraction/Controllers/CAN/CanController.h>
#include <hardware_abstraction/Controllers/I2C/I2CController.h>
#include <hardware_abstraction/Devices/ServoMotor/ServoMotorControl.h>

#include "business_logic/ImageCapturer3D/ImageCapturer3D.h"
#include "lwip/dhcp.h"
#include "lwip/init.h"
#include "lwip/sockets.h"
#include "lwipopts.h"

#include <business_logic/ClockSyncronization/TimeController.h>
#include <business_logic/ClockSyncronization/TimeBaseManager.h>
#include "application/SystemTasksManager/SystemTasksManager.h"
#include "services/Logger/LoggerMacros.h"

#include "business_logic/ImageCapturer3D/Image3DProjector.h"
using namespace hardware_abstraction::Controllers;
using namespace hardware_abstraction::Devices;
using namespace business_logic::Conectivity;
using namespace business_logic::Communication;
using namespace business_logic::ClockSyncronization;
using namespace business_logic;


static std::unique_ptr<PWMController> pwmVertCtrl;
static std::unique_ptr<ServoMotorControl> verServoControl;
static std::unique_ptr<PWMController> pwmHortCtrl;
static std::unique_ptr<ServoMotorControl> horServoControl;
static std::unique_ptr<ImageCapturer3D> image3dCapturer;
static std::unique_ptr<I2CController> i2cController;
static std::unique_ptr<GarminV3LiteCtrl> lidarDevice;
static std::shared_ptr<CommunicationManager> commMng;
static std::shared_ptr<SharedClockSlaveManager> globalClkMng;
static std::unique_ptr<application::SystemTasksManager> systemTaskHandler;

static std::shared_ptr<business_logic::ClockSyncronization::TimeController> timecontroller;
static std::shared_ptr<CanController> canController;
static std::shared_ptr<HTTPClient> httpClient;
application::TaskParams systemTaskMngParams;

void createHardwareAbstractionLayerComponents()
{
	PWMConfig pwmCfgVer;
	pwmVertCtrl = std::make_unique<PWMController>(pwmCfgVer);
	verServoControl = std::make_unique<ServoMotorControl>(std::move(pwmVertCtrl));

	PWMConfig pwmCfgHor;
	pwmCfgHor.pwmIndex = 0;
	pwmHortCtrl = std::make_unique<PWMController>(pwmCfgHor);
	horServoControl = std::make_unique<ServoMotorControl>(std::move(pwmHortCtrl));

	const auto garminLiteV3Addr = (0x62);
	LidarConfiguration lidarCfg{GarminV3LiteMode::Balance, garminLiteV3Addr};
	i2cController = std::make_unique<I2CController>();
	lidarDevice   = std::make_unique<GarminV3LiteCtrl>(std::move(i2cController), lidarCfg);

	canController = std::make_shared<CanController>();
	LOG_INFO("Created Hardware Abstraction layer components");
}

void createBusinessLogicLayerComponents()
{
	timecontroller = std::make_shared<TimeController>();
	commMng = std::make_shared<CommunicationManager>(timecontroller, canController);

	ImageCapturer3DConfig image3dConfig;
	image3dConfig.verServoCtrl = std::move(verServoControl);

	image3dConfig.horServoCtrl = std::move(horServoControl);
	image3dConfig.lidarCtrl = std::move(lidarDevice);
	image3dCapturer = std::make_unique<ImageCapturer3D>(image3dConfig);
	image3dCapturer->initialize();

	globalClkMng = std::make_shared<SharedClockSlaveManager>(timecontroller, canController);
	LOG_INFO("Created Business Logic layer components");


}

void createApplicationLayerComponents()
{
	systemTaskMngParams.image3dCapturer = std::move(image3dCapturer);
	systemTaskMngParams.globalClkMng    = globalClkMng;
	systemTaskMngParams.commMng         = commMng;
	systemTaskHandler = std::make_unique<application::SystemTasksManager>(std::move(systemTaskMngParams));
	systemTaskHandler->createPoolTasks();
	LOG_DEBUG("Created Application layer components");
}

void testPointProjector(std::vector<std::vector<float>> matrixA, std::vector<std::vector<float>> matrixB)
{
	auto image3dProjector = Image3DProjector();

    try {
        std::vector<std::vector<float>> result = image3dProjector.multiplyMatrix(matrixA, matrixB);

        std::cout << "Resultado de A x B:\n";
        for (const auto& row : result) {
            for (float value : row) {
                std::cout << value << " ";
            }
            std::cout << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

}

int main()
{

	LOG_INFO("*********************Starting Lidar Node Zybo Z7*********************");

//    std::vector<std::vector<float>> matrixA = {
//        {1.0f, 2.0f},
//        {3.0f, 4.0F}
//    };
//    std::vector<std::vector<float>> matrixB = {
//            {2.0f, 0.0f},
//            {1.0f, 3.0F}
//        };
//    testPointProjector(matrixA, matrixB);
//
//    std::vector<std::vector<float>> matrixC = {
//        {1.0f, 2.0f, 3.0},
//        {4.0f, 5.0f, 6.0},
//    };
//    std::vector<std::vector<float>> matrixD = {
//        {7.0f, 8.0f},
//        {9.0f, 10.0f},
//        {11.0f, 12.0f}
//    };
//    testPointProjector(matrixC, matrixD);
//
//    std::vector<std::vector<float>> matrixE = {
//        {1.0f, 2.0f, 3.0},
//        {4.0f, 5.0f, 6.0},
//        {7.0f, 8.0f, 9.0}
//    };
//    std::vector<std::vector<float>> matrixF = {
//        {9.0f, 8.0f, 7.0f},
//        {6.0f, 5.0f, 4.0f},
//        {3.0f, 2.0f, 1.0f}
//    };
//    testPointProjector(matrixE, matrixF);

	auto image3dProjector = Image3DProjector();
	CartesianLidarPoint lidarPoint(14,56,77);
	image3dProjector.project3DPointTo2D(lidarPoint);

	createHardwareAbstractionLayerComponents();
	createBusinessLogicLayerComponents();
	createApplicationLayerComponents();

	vTaskStartScheduler();
	while(1);
	return 0;
}
