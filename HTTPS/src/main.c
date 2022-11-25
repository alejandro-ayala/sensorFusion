

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "xparameters.h"
#include "netif/xadapter.h"
#include "Common/platform_config.h"
#include "xil_printf.h"
#include "Connection/ConnectionSettings.h"
#include "Connection/CryptoMng.h"
#include "Connection/HTTPConnectionTypes.h"
#include "lwip/init.h"
#include "lwip/sockets.h"
#include "lwipopts.h"
#include "FreeRTOS.h"
#include "task.h"
#include "mbedtls/platform.h"
#include "Connection/ServerManager.h"
#include "Communication/CommunicationManager.h"
#include "lwip/dhcp.h"
#include "HW/UserControlManager.h"
using namespace Conectivity;
using namespace Communication;
using namespace Hardware;

int main_thread();
int main_thread()
{
#if LWIP_DHCP==1
	int mscnt = 0;
#endif
	/* initialize lwIP before calling sys_thread_new */
    lwip_init();

    /* any thread using lwIP should be created using sys_thread_new */
    sys_thread_new("NW_THRD", network_thread, NULL,
		THREAD_STACKSIZE,
            DEFAULT_THREAD_PRIO);

#if LWIP_DHCP==1
    while (1) {
	vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);
		if (server_netif.ip_addr.addr) {
			xil_printf("DHCP request success\r\n");
			print_ip_settings(&(server_netif.ip_addr), &(server_netif.netmask), &(server_netif.gw));
		    int exit_code;

		    if((exit_code = mbedtls_platform_setup(NULL)) != 0) {
		        printf("Platform initialization failed with error %d\r\n", exit_code);
		        return MBEDTLS_EXIT_FAILURE;
		    }

		    mbedtls_printf("Starting mbed-os-example-tls/tls-client\n");



		    mbedtls_platform_teardown(NULL);
		}
		mscnt += DHCP_FINE_TIMER_MSECS;
		if (mscnt >= DHCP_COARSE_TIMER_SECS * 2000) {
			xil_printf("ERROR: DHCP request timed out\r\n");
			xil_printf("Configuring default IP of 192.168.1.10\r\n");
			IP4_ADDR(&(server_netif.ip_addr),  192, 168, 1, 10);
			IP4_ADDR(&(server_netif.netmask), 255, 255, 255,  0);
			IP4_ADDR(&(server_netif.gw),  192, 168, 1, 1);
			print_ip_settings(&(server_netif.ip_addr), &(server_netif.netmask), &(server_netif.gw));
			/* print all application headers */
			xil_printf("\r\n");
			xil_printf("%20s %6s %s\r\n", "Server", "Port", "Connect With..");
			xil_printf("%20s %6s %s\r\n", "--------------------", "------", "--------------------");

			xil_printf("\r\n");

			break;
		}
	}
#endif
    vTaskDelete(NULL);
    return 0;
}
TaskHandle_t task1Handle = NULL;
TaskHandle_t task2Handle = NULL;
void CommunicationTask(void *argument)
{
	CommunicationManager* commMng = reinterpret_cast<CommunicationManager*>(argument);
	TickType_t xLastWakeTime;

	xLastWakeTime = xTaskGetTickCount();


  /* Infinite loop */
	while(1)
	{
		commMng->selfTest();
		xil_printf("\r\nCommunicationTask wakeup\r\n");
		commMng->selfTest();
		//vTaskDelayUntil(xLastWakeTime,5000 / portTICK_RATE_MS);
		vTaskDelay(5000 / portTICK_RATE_MS);
	}
}
void UpdateConfigurationTask(void *argument)
{
	ServerManager* serverMng = reinterpret_cast<ServerManager*>(argument);
	TickType_t xLastWakeTime;
	std::string configStr;
	xLastWakeTime = xTaskGetTickCount();
	while(1)
	{
		xil_printf("\r\nUpdateConfigurationTask wakeup\r\n");
		configStr = serverMng->getConfiguration();
		xil_printf("\r\nconfiguration");
		xil_printf(configStr.c_str());
		vTaskDelay(20000 / portTICK_RATE_MS);
	}
}

void SendReportTask(void *argument)
{
	ServerManager* serverMng = reinterpret_cast<ServerManager*>(argument);
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	while(1)
	{
		xil_printf("\r\nSendReportTask wakeup\r\n");
		serverMng->sendReport();
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

void userIfaceControlTask(void *argument)
{
	UserControlManager* userControlMng = reinterpret_cast<UserControlManager*>(argument);
	userControlMng->initialization();
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	while(1)
	{
		xil_printf("\r\nSendReportTask wakeup\r\n");
		userControlMng->selfTest();
		vTaskDelay(300 / portTICK_RATE_MS);
	}
}

int main()
{
	TaskHandle_t updateConfigHandle = NULL;
	TaskHandle_t communicationHandle = NULL;
	TaskHandle_t sendingHandle = NULL;
	TaskHandle_t userIfHandle = NULL;


	static UserControlManager* userControlMng = new UserControlManager();

	static ServerManager* serverMng = new ServerManager();
	static CommunicationManager* commMng = new CommunicationManager();

	xTaskCreate( CommunicationTask, "CommunicationTask",THREAD_STACKSIZE,commMng,DEFAULT_THREAD_PRIO,&communicationHandle );
	//xTaskCreate( UpdateConfigurationTask, "UpdateConfigurationTask",THREAD_STACKSIZE,serverMng,DEFAULT_THREAD_PRIO,&updateConfigHandle );
	//xTaskCreate( SendReportTask, "SendReportTask",THREAD_STACKSIZE,serverMng,DEFAULT_THREAD_PRIO,&sendingHandle );
	xTaskCreate( userIfaceControlTask, "UserIfaceControlTask",THREAD_STACKSIZE,userControlMng,DEFAULT_THREAD_PRIO,&userIfHandle);

	sys_thread_new("main_thrd", (void(*)(void*))main_thread, 0,THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);

	vTaskStartScheduler();
	while(1);
	return 0;
}
