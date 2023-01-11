

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "xparameters.h"
#include "Common/platform_config.h"
#include "xil_printf.h"


#include "FreeRTOS.h"
#include "task.h"
#include "Communication/CommunicationManager.h"

using namespace Communication;
using namespace Hardware;

void CommunicationTask(void *argument)
{
	CanController* canController = reinterpret_cast<CanController*>(argument);
	TickType_t xLastWakeTime;

	xLastWakeTime = xTaskGetTickCount();
	xil_printf("\r\nCommunicationTask init\r\n");

	canController->initialize();
	uint8_t cnt = 0;
	uint8_t lenght = 800;
	uint8_t data[lenght];
  /* Infinite loop */
	while(1)
	{
		xil_printf("\r\nCommunicationTask send frame\r\n");

		canController->receiveMsg();

		vTaskDelay(50 / portTICK_RATE_MS);
	}
}

int main()
{
	TaskHandle_t communicationHandle = NULL;
	xil_printf("\r\nCommunicationTask start\r\n");

	static CanController* canController = new CanController();
	xTaskCreate(CommunicationTask, "CommunicationTask",THREAD_STACKSIZE,canController,tskIDLE_PRIORITY,&communicationHandle );

	vTaskStartScheduler();
	while(1);
	return 0;
}
