

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "xparameters.h"
#include "Common/platform_config.h"
#include "xil_printf.h"

#include "FreeRTOS.h"
#include "task.h"
#include "Communication/CommunicationManager.h"
#include "ClockSyncronization/SharedClockSlaveManager.h"
#include "Communication/CanController.h"
#include "ClockSyncronization/TimeController.h"
#include "IData.h"
using namespace Communication;
using namespace ClockSyncronization;
static bool gtSynq = false;
using namespace Hardware;

void getGlobalTimeTask(void *argument)
{
	SharedClockSlaveManager* slaveClkMng = reinterpret_cast<SharedClockSlaveManager*>(argument);
	TickType_t xLastWakeTime;
	slaveClkMng->initialization();
	xLastWakeTime = xTaskGetTickCount();
	xil_printf("\r\getGlobalTimeTask init\r\n");

  /* Infinite loop */
	while(1)
	{
		auto syncTime = slaveClkMng->getGlobalTime();
		if(syncTime)
		{
			gtSynq = true;
		}
	vTaskDelay(1 / portTICK_RATE_MS);
	}
}


void CommunicationTask(void *argument)
{
	CommunicationManager* commMng = reinterpret_cast<CommunicationManager*>(argument);
	TickType_t xLastWakeTime;
	xil_printf("\r\CommunicationTask init\r\n");

	xLastWakeTime = xTaskGetTickCount();
	uint32_t cnt = 0;
  /* Infinite loop */
	while(1)
	{
		if(gtSynq)
		{
			IData sensorData;
			sensorData.typeMsg = 0xE3;
			sensorData.data[0] = 0x13;
			sensorData.data[1] = 0x14;
			sensorData.data[2] = 0x15;
			sensorData.data[3] = 0x16;
			sensorData.sizeData = 4;
			sensorData.secCounter = cnt;
			sensorData.crc = 0xF1;
			commMng->sendData(sensorData);
			cnt++;
			//vTaskDelayUntil(xLastWakeTime,5000 / portTICK_RATE_MS);
			vTaskDelay(1000 / portTICK_RATE_MS);
		}
		else
			vTaskDelay(300 / portTICK_RATE_MS);
	}
}

int main()
{
	TaskHandle_t slaveClkMngHandle = NULL;
	TaskHandle_t communicationHandle = NULL;

	static TimeController*  timecontroller = new TimeController();
	static CanController*   canController = new CanController();
	static SharedClockSlaveManager* slaveClkMng = new SharedClockSlaveManager(timecontroller,canController);
	static CommunicationManager* commMng = new CommunicationManager(timecontroller,canController);

	xTaskCreate(getGlobalTimeTask, "GetGlobalTimeTask",THREAD_STACKSIZE,slaveClkMng,tskIDLE_PRIORITY,&slaveClkMngHandle );

	//xTaskCreate( CommunicationTask, "CommunicationTask",THREAD_STACKSIZE,commMng,tskIDLE_PRIORITY,&communicationHandle );

	vTaskStartScheduler();
	while(1);
	return 0;
}
