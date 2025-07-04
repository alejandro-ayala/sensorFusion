#include "VerificationTools.h"
#include "sleep.h"
XGpioPs Gpio;  // GPIO instance
XGpioPs gpio;
/*************************EXTERNAL IRQ SETTING**************************************/
extern void GpioIntrHandler(void *CallbackRef, u32 Bank, u32 Status);
void externalGpioConfiguration()
{
	auto xstatus = GpioSetup();
	if (xstatus != XST_SUCCESS) {
		print("GPIO setup failed\r\n");
	    return XST_FAILURE;
	}

	xstatus = ScuGicInterrupt_Init();

	if (xstatus != XST_SUCCESS) {
		print("ScuGicInterrupt_Init failed\r\n");
		return XST_FAILURE;
	}
}

int GpioSetup()
{
    XGpioPs_Config *ConfigPtr;
    int Status;

    ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
    if (ConfigPtr == NULL)
        return XST_FAILURE;

    Status = XGpioPs_CfgInitialize(&Gpio, ConfigPtr, ConfigPtr->BaseAddr);
    if (Status != XST_SUCCESS)
        return XST_FAILURE;

    XGpioPs_SetDirectionPin(&Gpio, GPIO_PIN, 0);  // Input
    XGpioPs_SetOutputEnablePin(&Gpio, GPIO_PIN, 0);

    XGpioPs_IntrEnablePin(&Gpio, GPIO_PIN);       // Enable interrupt for pin
    XGpioPs_IntrEnable(&Gpio, 0, (1 << GPIO_PIN)); // Bank 0, MIO
    XGpioPs_SetIntrTypePin(&Gpio, GPIO_PIN, XGPIOPS_IRQ_TYPE_EDGE_RISING);

    XGpioPs_SetCallbackHandler(&Gpio, (void *)&Gpio, GpioIntrHandler);
    XGpioPs_IntrClearPin(&Gpio, GPIO_PIN);  // Clear old interrupts

    //MIO12 para señalizar envio GlobalSync
   	XGpioPs_SetDirectionPin(&Gpio, 12, 1);
   	XGpioPs_SetOutputEnablePin(&Gpio, 12, 1);
   	XGpioPs_WritePin(&Gpio, 12, 1);
   	XGpioPs_WritePin(&Gpio, 12, 0);
   	toogleGpio();
    return XST_SUCCESS;
}
int ScuGicInterrupt_Init()
{
	int Status = XST_SUCCESS;
	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 * */
//	Xil_ExceptionInit();

	GicConfig = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);
	if (NULL == GicConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(&xInterruptController, GicConfig, GicConfig->CpuBaseAddress);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Setup the Interrupt System
	 * */

	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the ARM processor.
	 */
//	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
//			(Xil_ExceptionHandler) XScuGic_InterruptHandler,
//			(void *) &InterruptController);


	/*
	 * Connect a device driver handler that will be called when an
	 * interrupt for the device occurs, the device driver handler performs
	 * the specific interrupt processing for the device
	 */
	Status = XScuGic_Connect(&xInterruptController, GPIO_INTERRUPT_ID,
	         (Xil_ExceptionHandler)XGpioPs_IntrHandler,
	         (void *)&Gpio);

	if (Status != XST_SUCCESS) return XST_FAILURE;

	XScuGic_Enable(&xInterruptController, GPIO_INTERRUPT_ID);


	/*
	 * Enable interrupts in the ARM
	 */
//	Xil_ExceptionEnable();

	//Only used for edge sensitive Interrupts
	XScuGic_SetPriorityTriggerType(&xInterruptController, 90, 0xa0, 3);

	XScuGic_SetPriorityTriggerType(&xInterruptController, 91, 0xa0, 3);

	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
}

void toogleGpio()
{
   	XGpioPs_WritePin(&Gpio, 12, 1);
   	usleep(100);
   	XGpioPs_WritePin(&Gpio, 12, 0);
}
