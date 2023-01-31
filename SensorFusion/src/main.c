

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "xparameters.h"
#include "netif/xadapter.h"
#include "Common/platform_config.h"
#include "xil_printf.h"
#include "Conectivity/ConnectionSettings.h"
#include "Conectivity/CryptoMng.h"
#include "Conectivity/HTTPConnectionTypes.h"

#include "FreeRTOS.h"
#include "task.h"
#include "mbedtls/platform.h"
#include "Conectivity/ServerManager.h"
#include "Communication/CommunicationManager.h"
#include "Communication/CanController.h"

#include "HW/UserControlManager.h"
#include "HW/MotorController.h"
#include "HW/L298Hbridge.h"

#include "lwip/dhcp.h"
#include "lwip/init.h"
#include "lwip/sockets.h"
#include "lwipopts.h"

#include "HW/OLEDController.h"
#include "ClockSyncronization/TimeController.h"
#include "ClockSyncronization/TimeBaseManager.h"
using namespace Conectivity;
using namespace Communication;
using namespace Hardware;
using namespace ClockSyncronization;
void clockSyncTask(void *argument);

void CommunicationTask(void *argument)
{
	CommunicationManager* commMng = reinterpret_cast<CommunicationManager*>(argument);
	TickType_t xLastWakeTime;

	xLastWakeTime = xTaskGetTickCount();


  /* Infinite loop */
	while(1)
	{
		commMng->receiveData();
		//vTaskDelayUntil(xLastWakeTime,5000 / portTICK_RATE_MS);
		vTaskDelay(5 / portTICK_RATE_MS);
	}
}


void clockSyncTask(void *argument)
{
	TimeBaseManager* timeBaseMng = reinterpret_cast<TimeBaseManager*>(argument);
	xil_printf("\r\clockSyncTask\r\n");

	timeBaseMng->initialization();
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	while(1)
	{
		timeBaseMng->sendGlobalTime();
		vTaskDelay(300 / portTICK_RATE_MS);
	}
}

int main()
{
	TaskHandle_t updateConfigHandle = NULL;
	TaskHandle_t communicationHandle = NULL;
	TaskHandle_t sendingHandle = NULL;
	TaskHandle_t userIfHandle = NULL;
	TaskHandle_t motorControllerHandle = NULL;
	TaskHandle_t timeBaseMngHandle = NULL;
	static UserControlManager* userControlMng = new UserControlManager();

	//TODO add a factory pattern to get HBridge controller
	static PWMController* pwmController = new PWMController();
	static L298Hbridge*  hbridge = new L298Hbridge(pwmController);
	static MotorController* motorController = new MotorController(hbridge);

	//static ServerManager* serverMng = new ServerManager();
	//xTaskCreate( UpdateConfigurationTask, "UpdateConfigurationTask",THREAD_STACKSIZE,serverMng,DEFAULT_THREAD_PRIO,&updateConfigHandle );
	//xTaskCreate( SendReportTask, "SendReportTask",THREAD_STACKSIZE,serverMng,DEFAULT_THREAD_PRIO,&sendingHandle );
	//xTaskCreate( userIfaceControlTask, "UserIfaceControlTask",THREAD_STACKSIZE,userControlMng,DEFAULT_THREAD_PRIO,&userIfHandle);
	//xTaskCreate(motorControlTask, "MotorControlTask",THREAD_STACKSIZE,motorController,DEFAULT_THREAD_PRIO,&motorControllerHandle);

	//sys_thread_new("main_thrd", (void(*)(void*))main_thread, 0,THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
	static TimeController*  timecontroller = new TimeController();
	static CanController*   canController = new CanController();
	static HTTPClient*      httpClient = new HTTPClient();
	static TimeBaseManager* timeBaseMng = new TimeBaseManager(timecontroller,canController,httpClient);
	xTaskCreate(clockSyncTask, "ClockSyncTask",THREAD_STACKSIZE,timeBaseMng,DEFAULT_THREAD_PRIO,&timeBaseMngHandle);

	static CommunicationManager* commMng = new CommunicationManager(timecontroller,canController);
	//xTaskCreate( CommunicationTask, "CommunicationTask",THREAD_STACKSIZE,commMng,DEFAULT_THREAD_PRIO,&communicationHandle );

	vTaskStartScheduler();
	while(1);
	return 0;
}
