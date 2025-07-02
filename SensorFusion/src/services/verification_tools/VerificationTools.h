extern  "C" {
#include "xgpiops.h"
#include <stdio.h>
#include "xil_io.h"
#include "xil_types.h"
#include "xparameters.h"
#include "xil_io.h"
#include "xil_exception.h"
#include "xscugic.h"

#define GPIO_DEVICE_ID      XPAR_XGPIOPS_0_DEVICE_ID
#define GPIO_INTERRUPT_ID   XPAR_XGPIOPS_0_INTR
#define GPIO_PIN            9   // MIO9


extern XScuGic xInterruptController;
static XScuGic_Config *GicConfig;/* The configuration parameters of the controller */

void externalGpioConfiguration();
int GpioSetup();
int ScuGicInterrupt_Init();
}
