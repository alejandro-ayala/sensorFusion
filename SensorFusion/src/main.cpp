
#define APP
#ifdef APP

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

using namespace hardware_abstraction::Controllers;
using namespace hardware_abstraction::Devices;
using namespace business_logic::Conectivity;
using namespace business_logic::Communication;
using namespace business_logic::ClockSyncronization;
using namespace business_logic;

void clockSyncTask(void *argument);

uint8_t buffer[1];
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
		vTaskDelay(10000 / portTICK_RATE_MS);
	}
}

int main()
{
	TaskHandle_t updateConfigHandle = NULL;
	TaskHandle_t communicationHandle = NULL;
	TaskHandle_t sendingHandle = NULL;
	TaskHandle_t userIfHandle = NULL;
	TaskHandle_t MotorControlHandle = NULL;
	TaskHandle_t timeBaseMngHandle = NULL;

	ImageCapturer3DConfig image3dConfig;
	static std::shared_ptr<ImageCapturer3D> image3dCapturer = std::make_shared<ImageCapturer3D>(image3dConfig);
	image3dCapturer->initialize();
	image3dCapturer->captureImage();

//	PWMConfig pwmCfg1;
//	PWMConfig pwmCfg2;
//	static std::shared_ptr<PWMController> pwmController = std::make_shared<PWMController>(pwmCfg1);
//	//static std::shared_ptr<ServoMotorControl> servoControl = std::make_shared<ServoMotorControl>(pwmController);
//
//	pwmCfg2.pwmFreq = 1;
//	pwmCfg2.pwmIndex = 0;
//	static std::shared_ptr<PWMController> pwmController1 = std::make_shared<PWMController>(pwmCfg2);
//	//static std::shared_ptr<ServoMotorControl> servoControl1 = std::make_shared<ServoMotorControl>(pwmController);

//	pwmController->initialize();
	//pwmController1->initialize();

//	servoControl->setAngle(0);
//	auto angle = servoControl->getAngle();
//
//	servoControl->setAngle(90);
//    angle = servoControl->getAngle();
//
//	servoControl->setAngle(180);
//	angle = servoControl->getAngle();
//	pwmController->setDutyCicle(0);
//	pwmController->setDutyCicle(25);
//	pwmController->setDutyCicle(50);
//	pwmController->setDutyCicle(75);
//	pwmController->setDutyCicle(100);

//	pwmController1->setDutyCicle(0);
//	pwmController1->setDutyCicle(25);
//	pwmController1->setDutyCicle(50);
//	pwmController1->setDutyCicle(75);
//	pwmController1->setDutyCicle(100);
//	bool changeFreq = false;
//	bool changeDT = false;
//	uint8_t freq;
//	uint8_t dt;
//	while(1)
//	{
//		if(changeFreq)
//		{
//			pwmController->setFrequency(freq);
//			//pwmController1->setFrequency(freq);
//		}
//		if(changeDT)
//		{
//			pwmController->setDutyCicle(dt);
//			//pwmController1->setDutyCicle(dt);
//		}
//	}
	//static ServerManager* serverMng = new ServerManager();
	//xTaskCreate( UpdateConfigurationTask, "UpdateConfigurationTask",THREAD_STACKSIZE,serverMng,DEFAULT_THREAD_PRIO,&updateConfigHandle );
	//xTaskCreate( SendReportTask, "SendReportTask",THREAD_STACKSIZE,serverMng,DEFAULT_THREAD_PRIO,&sendingHandle );
	//xTaskCreate( userIfaceControlTask, "UserIfaceControlTask",THREAD_STACKSIZE,userControlMng,DEFAULT_THREAD_PRIO,&userIfHandle);
	//xTaskCreate(motorControlTask, "MotorControlTask",THREAD_STACKSIZE,MotorControl,DEFAULT_THREAD_PRIO,&MotorControlHandle);

	//sys_thread_new("main_thrd", (void(*)(void*))main_thread, 0,THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
	static TimeController*  timecontroller = new TimeController();
	static CanController*   canController = new CanController();
	static HTTPClient*      httpClient = new HTTPClient();


	const auto garminLiteV3Addr = (0x62);
	LidarConfiguration lidarCfg{GarminV3LiteMode::Balance, garminLiteV3Addr};
	auto i2cController = std::make_shared<I2CController>();

	//i2cController->readData(0x08, 0x17, buffer, 1);
	auto lidarDevice   = std::make_shared<GarminV3LiteCtrl>(i2cController, lidarCfg);

	lidarDevice->initialization();
	while(1)
	{
		const auto distance = lidarDevice->readDistance();
	    std::cout << "Distance: 0x" << std::hex << std::uppercase
	              << static_cast<int>(distance) << std::endl;
		//vTaskDelay(500 / portTICK_RATE_MS);
		for (uint32_t Delay = 0; Delay < 0xFFFFFF; Delay++);
	}



	static TimeBaseManager* timeBaseMng = new TimeBaseManager(timecontroller,canController,httpClient);
	xTaskCreate(clockSyncTask, "ClockSyncTask",THREAD_STACKSIZE,timeBaseMng,DEFAULT_THREAD_PRIO,&timeBaseMngHandle);

	static CommunicationManager* commMng = new CommunicationManager(timecontroller,canController);
	xTaskCreate( CommunicationTask, "CommunicationTask",THREAD_STACKSIZE,commMng,DEFAULT_THREAD_PRIO,&communicationHandle );

	vTaskStartScheduler();
	while(1);
	return 0;
}

#else

int distance;


/******************************************************************************
* Copyright (C) 2010 - 2021 Xilinx, Inc.  All rights reserved.
* Copyright (C) 2022 - 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
 * @file xiicps_intr_master_example.c
 *
 * This example can run on zynqmp / versal platform evaluation board and
 * IIC controller configured master in interrupt-driven mode and Aardvark
 * Analyzer used as slave.
 *
 * It sends 18 buffers of data to slave and expects to receive the
 * same data through the IIC using the Aardvark test hardware.
 *
 * <pre> MODIFICATION HISTORY:
 *
 * Ver   Who Date     Changes
 * ----- --- -------- -----------------------------------------------
 * 1.00a jz  01/30/10 First release
 * 3.18  gm  07/14/23 Added SDT support.
 *
 * </pre>
 *
 ****************************************************************************/

/***************************** Include Files **********************************/
#include "xparameters.h"
#include "xiicps.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"
#ifdef SDT
#include "xinterrupt_wrap.h"
#endif

/************************** Constant Definitions ******************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#ifndef SDT
#define IIC_DEVICE_ID		XPAR_XIICPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define IIC_INT_VEC_ID		XPAR_XIICPS_0_INTR
#else
#define XIICPS_BASEADDRESS	XPAR_XIICPS_0_BASEADDR
#endif

/*
 * The slave address to send to and receive from.
 */
#define IIC_SLAVE_ADDR		0x8//(0x62)//<<1)//0x55
#define IIC_SCLK_RATE		100000

#include <iostream>
/*
 * The following constant controls the length of the buffers to be sent
 * and received with the IIC.
 */
#define TEST_BUFFER_SIZE   	250
#define NUMBER_OF_SIZES		18

/**************************** Type Definitions ********************************/

/************************** Function Prototypes *******************************/

#ifndef SDT
int IicPsMasterIntrExample(u16 DeviceId);
static int SetupInterruptSystem(XIicPs *IicPsPtr);
#else
int IicPsMasterIntrExample(UINTPTR BaseAddress);
#endif

void Handler(void *CallBackRef, u32 Event);

/************************** Variable Definitions ******************************/

XIicPs Iic;			/* Instance of the IIC Device */
#ifndef SDT
XScuGic InterruptController;	/* Instance of the Interrupt Controller */
#endif

/*
 * The following buffers are used in this example to send and receive data
 * with the IIC. They are defined as global so that they are not on the stack.
 */
u8 SendBuffer[TEST_BUFFER_SIZE];    /* Buffer for Transmitting Data */
u8 RecvBuffer[TEST_BUFFER_SIZE];    /* Buffer for Receiving Data */

/*
 * The following counters are used to determine when the entire buffer has
 * been sent and received.
 */
volatile u32 SendComplete;
volatile u32 RecvComplete;
volatile u32 TotalErrorCount;

/******************************************************************************/
/**
*
* Main function to call the example.
*
*
* @return	XST_SUCCESS if successful, XST_FAILURE if unsuccessful.
*
* @note		None.
*
*******************************************************************************/

void readSN()
{
	uint8_t cmd[1];
	while (XIicPs_BusIsBusy(&Iic)) {}
	SendComplete = FALSE;
	cmd[0] = 0x16;
	XIicPs_MasterSendPolled(&Iic, cmd, 1, IIC_SLAVE_ADDR);
	//while (!SendComplete) {}
	while (XIicPs_BusIsBusy(&Iic)) {}

	uint8_t sn[2];
	RecvComplete = FALSE;
	XIicPs_MasterRecvPolled(&Iic, sn, 2, IIC_SLAVE_ADDR);


	//while (!RecvComplete) {}
	while (XIicPs_BusIsBusy(&Iic)) {}
	if(sn[0] != 0)
	{
		std::cout << "SN: " ;
	}
}

void lidarInit()
{

	uint8_t cmd[1];
	while (XIicPs_BusIsBusy(&Iic)) {}
	SendComplete = FALSE;
	XIicPs_MasterSendPolled(&Iic, cmd, 1, IIC_SLAVE_ADDR);
	//while (!SendComplete) {}
	while (XIicPs_BusIsBusy(&Iic)) {}

	/*----------CONFIGURATION LIDAR----------*/
	cmd[0]=0xff;
	SendComplete = FALSE;
	XIicPs_MasterSendPolled(&Iic, cmd, 1, IIC_SLAVE_ADDR);
	//while (!SendComplete) {}
	while (XIicPs_BusIsBusy(&Iic)) {}
	cmd[0]=0x08;
	SendComplete = FALSE;
	XIicPs_MasterSendPolled(&Iic, cmd, 1, IIC_SLAVE_ADDR);
	//while (!SendComplete) {}
	while (XIicPs_BusIsBusy(&Iic)) {}
	cmd[0]=0x00;
	SendComplete = FALSE;
	XIicPs_MasterSendPolled(&Iic, cmd, 1, IIC_SLAVE_ADDR);
	//while (!SendComplete) {}
	while (XIicPs_BusIsBusy(&Iic)) {}
}

void lidarRead()
{
	uint8_t cmd[1];
	uint8_t data[2];
  	cmd[0] = 0x00;
  	cmd[1] = 0x04;
	SendComplete = FALSE;
	XIicPs_MasterSendPolled(&Iic, cmd, 2, IIC_SLAVE_ADDR);
	//while (!SendComplete) {}
	while (XIicPs_BusIsBusy(&Iic)) {}

  	cmd[0] = 0x01;
	data[0] = 0x1;
	SendComplete = FALSE;
	XIicPs_MasterSendPolled(&Iic, cmd, 1, IIC_SLAVE_ADDR);
	while (XIicPs_BusIsBusy(&Iic)) {}
	auto res = XIicPs_MasterRecvPolled(&Iic, data, 1, IIC_SLAVE_ADDR);
	while(!(data && 0x01))
	{
		auto res = XIicPs_MasterRecvPolled(&Iic, data, 1, IIC_SLAVE_ADDR);
	}

	cmd[0] = 0x0f;
	SendComplete = FALSE;
	XIicPs_MasterSendPolled(&Iic, cmd, 1, IIC_SLAVE_ADDR);
	//while (!SendComplete) {}
	while (XIicPs_BusIsBusy(&Iic)) {}

	RecvComplete = FALSE;
	res = XIicPs_MasterRecvPolled(&Iic, data, 2, IIC_SLAVE_ADDR);


	//while (!RecvComplete) {}
	while (XIicPs_BusIsBusy(&Iic)) {}


  	distance =(data[0]<<8)|(data[1]);



}

int main(void)
{
	int Status;

	xil_printf("IIC Master Interrupt Example Test \r\n");

	/*
	 * Run the Iic Master Interrupt example , specify the Device ID that is
	 * generated in xparameters.h
	 */
#ifndef SDT
	Status = IicPsMasterIntrExample(IIC_DEVICE_ID);
#else
	Status = IicPsMasterIntrExample(XIICPS_BASEADDRESS);
#endif
	if (Status != XST_SUCCESS) {
		xil_printf("IIC Master Interrupt Example Test Failed\r\n");
		return XST_FAILURE;
	}

	xil_printf("Successfully ran IIC Master Interrupt Example Test\r\n");
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function does a minimal test on the Iic device and driver as a
* design example. The purpose of this function is to illustrate
* how to use the XIicPs driver.
*
* This function sends data and expects to receive the same data through the IIC
* using the Aardvark test hardware.
*
* This function uses interrupt driver mode of the IIC.
*
* @param	DeviceId is the Device ID of the IicPs Device and is the
*		XPAR_<IICPS_instance>_DEVICE_ID value from xparameters.h
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note
*
* This function contains an infinite loop such that if interrupts are not
* working it may never return.
*
*******************************************************************************/
#ifndef SDT
int IicPsMasterIntrExample(u16 DeviceId)
#else
int IicPsMasterIntrExample(UINTPTR BaseAddress)
#endif
{
	int Status;
	XIicPs_Config *Config;

	/*
	 * Initialize the IIC driver so that it's ready to use
	 * Look up the configuration in the config table, then initialize it.
	 */
#ifndef SDT
	Config = XIicPs_LookupConfig(DeviceId);
#else
	Config = XIicPs_LookupConfig(BaseAddress);
#endif
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XIicPs_CfgInitialize(&Iic, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XIicPs_SelfTest(&Iic);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the IIC to the interrupt subsystem such that interrupts can
	 * occur. This function is application specific.
	 */
	Status = SetupInterruptSystem(&Iic);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Setup the handlers for the IIC that will be called from the
	 * interrupt context when data has been sent and received, specify a
	 * pointer to the IIC driver instance as the callback reference so
	 * the handlers are able to access the instance data.
	 */
//	XIicPs_SetStatusHandler(&Iic, (void *) &Iic, Handler);

	/*
	 * Set the IIC serial clock rate.
	 */
	XIicPs_SetSClk(&Iic, IIC_SCLK_RATE);


	lidarInit();

	readSN();
	while(1){
		lidarRead();
		if(distance != 0)
		{
			std::cout << "distance !!!" << std::endl;
		}

		else
		{
			std::cout << "Error !!!" << std::endl;

		}

	}
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function is the handler which performs processing to handle data events
* from the IIC.  It is called from an interrupt context such that the amount
* of processing performed should be minimized.
*
* This handler provides an example of how to handle data for the IIC and
* is application specific.
*
* @param	CallBackRef contains a callback reference from the driver, in
*		this case it is the instance pointer for the IIC driver.
* @param	Event contains the specific kind of event that has occurred.
*
* @return	None.
*
* @note		None.
*
*******************************************************************************/
void Handler(void *CallBackRef, u32 Event)
{
	/*
	 * All of the data transfer has been finished.
	 */
	if (0 != (Event & XIICPS_EVENT_COMPLETE_RECV)) {
		RecvComplete = TRUE;
	} else if (0 != (Event & XIICPS_EVENT_COMPLETE_SEND)) {
		SendComplete = TRUE;
	} else if (0 == (Event & XIICPS_EVENT_SLAVE_RDY)) {
		/*
		 * If it is other interrupt but not slave ready interrupt, it is
		 * an error.
		 * Data was received with an error.
		 */
		TotalErrorCount++;
	}
}

#ifndef SDT
/******************************************************************************/
/**
*
* This function setups the interrupt system such that interrupts can occur
* for the IIC.  This function is application specific since the actual
* system may or may not have an interrupt controller.  The IIC could be
* directly connected to a processor without an interrupt controller.  The
* user should modify this function to fit the application.
*
* @param	IicPsPtr contains a pointer to the instance of the Iic
*		which is going to be connected to the interrupt controller.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
*******************************************************************************/
static int SetupInterruptSystem(XIicPs *IicPsPtr)
{
	int Status;
	XScuGic_Config *IntcConfig; /* Instance of the interrupt controller */

	Xil_ExceptionInit();

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(&InterruptController, IntcConfig,
				       IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
				     (Xil_ExceptionHandler)XScuGic_InterruptHandler,
				     &InterruptController);

	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	Status = XScuGic_Connect(&InterruptController, IIC_INT_VEC_ID,
				 (Xil_InterruptHandler)XIicPs_MasterInterruptHandler,
				 (void *)IicPsPtr);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	/*
	 * Enable the interrupt for the Iic device.
	 */
	XScuGic_Enable(&InterruptController, IIC_INT_VEC_ID);


	/*
	 * Enable interrupts in the Processor.
	 */
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}
#endif

#endif
