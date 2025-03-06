
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




#ifdef HTTP_CLIENT
static std::shared_ptr<HTTPClient> httpClient;
#endif

static std::shared_ptr<CommunicationManager> commMng;
static std::shared_ptr<TimeBaseManager> globalClkMng;
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
#ifdef HTTP_CLIENT
	httpClient = std::make_shared<HTTPClient>();
#endif
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

	LOG_INFO("*********************Starting SensorFusion Node Zybo Z7*********************");
	createHardwareAbstractionLayerComponents();
	createBusinessLogicLayerComponents();
	createApplicationLayerComponents();

	vTaskStartScheduler();
	while(1);
	return 0;
}
