#include <hardware_abstraction/Controllers/I2C/I2CController.h>
#include <iostream>
#include "services/Logger/LoggerMacros.h"
#include "xenv_standalone.h"

//#define I2C_POLLING
namespace hardware_abstraction
{
namespace Controllers
{

void I2CController::irqHandler(void *callBackRef, u32 event)
{
	/*
	 * All of the data transfer has been finished.
	 */
	if (0 != (event & XIICPS_EVENT_COMPLETE_RECV))
	{
		recvComplete = true;
	}
	else if (0 != (event & XIICPS_EVENT_COMPLETE_SEND))
	{
		sendComplete = true;
	}
	else if (0 == (event & XIICPS_EVENT_SLAVE_RDY))
	{
		/*
		 * If it is other interrupt but not slave ready interrupt, it is
		 * an error.
		 * Data was received with an error.
		 */
		totalErrorCount++;
		totalErrorCount = 0;
		//TODO handle error
		LOG_ERROR("I2C Error during irqHandler");
	}
}

I2CController::I2CController(const I2CConfig& config) : m_config(config)
{
	try
	{
		initialize();
	}
	catch(...)
	{
		//Handle error
	}
	
}

void I2CController::initialize()
{
	int Status;
	XIicPs_Config *Config;

	/*
	 * Initialize the IIC driver so that it's ready to use
	 * Look up the configuration in the config table, then initialize it.
	 */

	Config = XIicPs_LookupConfig(m_i2cDeviceId);

	if (NULL == Config)
	{
		//TODO handle error
		LOG_FATAL("XIicPs_LookupConfig XST_FAILURE");
	}

	Status = XIicPs_CfgInitialize(&m_config.i2cPsInstance, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS)
	{
		//TODO handle error
		LOG_FATAL("XIicPs_CfgInitialize XST_FAILURE");
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XIicPs_SelfTest(&m_config.i2cPsInstance);
	if (Status != XST_SUCCESS)
	{
		//TODO handle error
		LOG_FATAL("XIicPs_SelfTest XST_FAILURE");
	}

#ifndef I2C_POLLING
	/*
	 * Connect the IIC to the interrupt subsystem such that interrupts can
	 * occur. This function is application specific.
	 */
	Status = SetupInterruptSystem(&m_config.i2cPsInstance);

	if (Status != XST_SUCCESS)
	{
		//TODO handle error
		LOG_FATAL("SetupInterruptSystem XST_FAILURE");
	}

	/*
	 * Setup the handlers for the IIC that will be called from the
	 * interrupt context when data has been sent and received, specify a
	 * pointer to the IIC driver instance as the callback reference so
	 * the handlers are able to access the instance data.
	 */
	XIicPs_SetStatusHandler(&m_config.i2cPsInstance, (void *) &m_config.i2cPsInstance, irqHandler);
#endif
	/*
	 * Set the IIC serial clock rate.
	 */
	XIicPs_SetSClk(&m_config.i2cPsInstance, m_config.clockRate);
}

uint8_t I2CController::readData(uint8_t slaveAddr, uint8_t registerAddr,  uint8_t *buffer, uint8_t bufferSize)
{
	sendComplete = FALSE;
#ifndef I2C_POLLING
	XIicPs_MasterSend(&m_config.i2cPsInstance, &registerAddr, 1, slaveAddr);

	/*
	 * Wait for the entire buffer to be sent, letting the interrupt
	 * processing work in the background, this function may get
	 * locked up in this loop if the interrupts are not working
	 * correctly.
	 */
	while (!sendComplete)
	{
		if (0 != totalErrorCount)
		{
			//TODO handle error
			LOG_ERROR("I2C Error during sending register address");
		}
	}

	/*
	 * Wait bus activities to finish.
	 */
	while (XIicPs_BusIsBusy(&m_config.i2cPsInstance))
	{
		/* NOP */
	}

	/*
	 * Receive data from slave, errors are reported through
	 * totalErrorCount.
	 */
#else
	auto Status = XIicPs_MasterSendPolled(&m_config.i2cPsInstance, registerAddr, 1, slaveAddr);

	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
#endif
	udelay(5000);
#ifndef I2C_POLLING
	recvComplete = FALSE;
	XIicPs_MasterRecv(&m_config.i2cPsInstance, buffer, bufferSize, slaveAddr);


	while (!recvComplete)
	{
		if (0 != totalErrorCount)
		{
			//TODO handle error
			LOG_ERROR("I2C Error during receiving data register");
		}
	}

	/*
	 * Wait bus activities to finish.
	 */
	while (XIicPs_BusIsBusy(&m_config.i2cPsInstance))
	{
		/* NOP */
	}

#else
	Status = XIicPs_MasterRecvPolled(&m_config.i2cPsInstance, buffer, bufferSize, slaveAddr);

	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
#endif
}

uint8_t I2CController::sendData(uint8_t slaveAddr, uint8_t *buffer, uint32_t bufferSize)
{	
#ifndef I2C_POLLING
	XIicPs_MasterSend(&m_config.i2cPsInstance, buffer, bufferSize, slaveAddr);
				  	
	while (!sendComplete)
	{
		if (0 != totalErrorCount)
		{
			//TODO handle error
			LOG_ERROR("I2C Error during sending I2C data");
		}
	}

	while (XIicPs_BusIsBusy(&m_config.i2cPsInstance))
	{

	}
#else
	auto Status = XIicPs_MasterSendPolled(&m_config.i2cPsInstance, buffer, bufferSize, slaveAddr);

	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
#endif
	return XST_SUCCESS;
}

int I2CController::SetupInterruptSystem(XIicPs *IicPsPtr)
{
	int Status;
	XScuGic_Config *IntcConfig; /* Instance of the interrupt controller */

	Xil_ExceptionInit();

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(m_intCtrlDeviceId);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(&m_config.InterruptController, IntcConfig,
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
				     &m_config.InterruptController);

	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	Status = XScuGic_Connect(&m_config.InterruptController, m_i2cIntVectorId,
				 (Xil_InterruptHandler)XIicPs_MasterInterruptHandler,
				 (void *)IicPsPtr);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	/*
	 * Enable the interrupt for the Iic device.
	 */
	XScuGic_Enable(&m_config.InterruptController, m_i2cIntVectorId);


	/*
	 * Enable interrupts in the Processor.
	 */
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

bool I2CController::selfTest()
{
	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	const auto selfTestState = XIicPs_SelfTest(&m_config.i2cPsInstance);
	if (selfTestState != XST_SUCCESS) 
	{
		return false;
	}

	return true;
}
}
}
