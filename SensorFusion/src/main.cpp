
#ifdef HTTP_CLIENT
#include <business_logic/Conectivity/ConnectionSettings.h>
#include <business_logic/Conectivity/CryptoMng.h>
#include <business_logic/Conectivity/HTTPConnectionTypes.h>
#include <business_logic/Conectivity/ServerManager.h>
#include <business_logic/Conectivity/HTTPClient.h>
#include <business_logic/Image3DProjector/Image3DProjector.h>
#include "../external/mbedtls/include/mbedtls/platform.h"
#include "lwip/dhcp.h"
#include "lwip/init.h"
#include "lwip/sockets.h"
#include "lwipopts.h"
#include "netif/xadapter.h"
#include <platform_config.h>
#endif

#include <hardware_abstraction/Controllers/CAN/CanController.h>
#include <business_logic/Communication/CommunicationManager.h>
#include <business_logic/ClockSyncronization/TimeController.h>
#include <business_logic/ClockSyncronization/TimeBaseManager.h>
#include "application/SystemTasksManager/SystemTasksManager.h"
#include "services/Logger/LoggerMacros.h"

using namespace hardware_abstraction::Controllers;
using namespace hardware_abstraction::Devices;
using namespace business_logic::Communication;
using namespace business_logic::ClockSyncronization;
using namespace business_logic;



#ifdef HTTP_CLIENT
static std::shared_ptr<HTTPClient> httpClient;
#endif

static std::unique_ptr<PWMController> pwmVertCtrl;
static std::unique_ptr<ServoMotorControl> verServoControl;
static std::unique_ptr<PWMController> pwmHortCtrl;
static std::unique_ptr<ServoMotorControl> horServoControl;
static std::unique_ptr<ImageCapturer3D> image3dCapturer;
static std::unique_ptr<I2CController> i2cController;
static std::unique_ptr<GarminV3LiteCtrl> lidarDevice;

static std::shared_ptr<CommunicationManager> commMng;
static std::shared_ptr<TimeBaseManager> globalClkMng;
static std::unique_ptr<application::SystemTasksManager> systemTaskHandler;
static std::shared_ptr<business_logic::ClockSyncronization::TimeController> timecontroller;
static std::shared_ptr<PsCanController> canController;
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

	canController = std::make_shared<PsCanController>();
	LOG_INFO("Created Hardware Abstraction layer components");
}

void createBusinessLogicLayerComponents()
{
	timecontroller = std::make_shared<TimeController>();
	commMng = std::make_shared<CommunicationManager>(timecontroller, canController);
#ifdef HTTP_CLIENT
	httpClient = std::make_shared<HTTPClient>();
#endif
	globalClkMng = std::make_shared<TimeBaseManager>(timecontroller, canController);//, httpClient);



	ImageCapturer3DConfig image3dConfig;
	image3dConfig.verServoCtrl = std::move(verServoControl);
	image3dConfig.horServoCtrl = std::move(horServoControl);
	image3dConfig.lidarCtrl = std::move(lidarDevice);
	image3dCapturer = std::make_unique<ImageCapturer3D>(image3dConfig);
	image3dCapturer->initialize();

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
#include <hardware_abstraction/Controllers/CAN/PsCanController.h>

int main()
{
	LOG_INFO("*********************Starting SensorFusion Node Zybo Z7*********************");


	createHardwareAbstractionLayerComponents();
	createBusinessLogicLayerComponents();
	createApplicationLayerComponents();

	vTaskStartScheduler();
	while(1);
	return 0;
}
