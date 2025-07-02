//#define TENSORFLOW_LITE

#ifdef TENSORFLOW_LITE


#include "tensorflow/lite/micro/examples/person_detection/main_functions.h"

#include "tensorflow/lite/micro/examples/person_detection/detection_responder.h"
#include "tensorflow/lite/micro/examples/person_detection/image_provider.h"
#include "tensorflow/lite/micro/examples/person_detection/model_settings.h"
#include "tensorflow/lite/micro/examples/person_detection/person_detect_model_data.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

#include "tensorflow/lite/micro/examples/person_detection/main_functions.h"

// This is the default main used on systems that have the standard C entry
// point. Other devices (for example FreeRTOS or ESP32) that have different
// requirements for entry code (like an app_main function) should specialize
// this main.cc file in a target-specific subfolder.
int main(int argc, char* argv[]) {
  setup();
  while (true) {
    loop();
  }
}

#else
#ifdef HTTP_CLIENT
#include "business_logic/ServerManager/ConnectionSettings.h"
#include <business_logic/ServerManager/CryptoMng.h>
#include <business_logic/ServerManager/HTTPConnectionTypes.h>
#include <business_logic/ServerManager/ServerManager.h>
#include <business_logic/ServerManager/HTTPClient.h>
#include <business_logic/Image3DProjector/Image3DProjector.h>
#include "../external/mbedtls/include/mbedtls/platform.h"
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

static std::shared_ptr<business_logic::Osal::QueueHandler> cameraFramesQueue;

static std::shared_ptr<TimeBaseManager> globalClkMng;
static std::unique_ptr<application::SystemTasksManager> systemTaskHandler;
static std::shared_ptr<business_logic::ClockSyncronization::TimeController> timecontroller;
static std::shared_ptr<PsCanController> canController;

static std::shared_ptr<business_logic::ImageAssembler::SharedImage> sharedImage;
static std::shared_ptr<business_logic::ImageClassifier::ImageProvider> imageProvider;
static std::shared_ptr<business_logic::ImageAssembler::ImageAssembler> imageAssembler;

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
	LidarConfiguration lidarCfg{GarminV3LiteMode::DefaultRange, garminLiteV3Addr};
	i2cController = std::make_unique<I2CController>();
	lidarDevice   = std::make_unique<GarminV3LiteCtrl>(std::move(i2cController), lidarCfg);

	canController = std::make_shared<PsCanController>();
	LOG_INFO("Created Hardware Abstraction layer components");
}

void createBusinessLogicLayerComponents()
{
	timecontroller = std::make_shared<TimeController>();
	uint32_t queueItemSize   = sizeof(std::array<uint8_t, hardware_abstraction::Controllers::CAN_DATA_PAYLOAD_SIZE>);
	uint32_t queueLength     = 200;
	cameraFramesQueue = std::make_shared<business_logic::Osal::QueueHandler>(queueLength, queueItemSize);

#ifdef HTTP_CLIENT
	httpClient = std::make_shared<HTTPClient>();
#endif
	globalClkMng = std::make_shared<TimeBaseManager>(timecontroller, canController);//, httpClient);
	sharedImage  = std::make_shared<business_logic::ImageAssembler::SharedImage>();
	imageProvider = std::make_shared<business_logic::ImageClassifier::ImageProvider>(sharedImage);

	imageAssembler  = std::make_shared<business_logic::ImageAssembler::ImageAssembler>(cameraFramesQueue, imageProvider);

#ifndef ASSEMBLER_TASK
	commMng = std::make_shared<CommunicationManager>(timecontroller, canController, cameraFramesQueue, imageAssembler);
#else
	systemTaskMngParams.imageAssembler = imageAssembler;
	commMng = std::make_shared<CommunicationManager>(timecontroller, canController, cameraFramesQueue);
#endif


	ImageCapturer3DConfig image3dConfig;
	image3dConfig.verServoCtrl = std::move(verServoControl);
	image3dConfig.horServoCtrl = std::move(horServoControl);
	image3dConfig.lidarCtrl = std::move(lidarDevice);
	image3dCapturer = std::make_unique<ImageCapturer3D>(image3dConfig);

	LOG_INFO("Created Business Logic layer components");
}

void createApplicationLayerComponents()
{
	systemTaskMngParams.image3dCapturer = std::move(image3dCapturer);
	systemTaskMngParams.globalClkMng    = globalClkMng;
	systemTaskMngParams.commMng         = commMng;
	systemTaskMngParams.cameraFramesQueue = cameraFramesQueue;
	systemTaskMngParams.imageProvider = imageProvider;

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

#endif
