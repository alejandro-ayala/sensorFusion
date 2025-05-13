#include <hardware_abstraction/Controllers/I2C/I2CController.h>
#include "services/Exception/SystemExceptions.h"

#include <iostream>
#include "services/Logger/LoggerMacros.h"
#include "xenv_standalone.h"

namespace hardware_abstraction
{
namespace Controllers
{

#ifndef I2C_POLLING
void I2CController::irqHandler(void *callBackRef, u32 event)
{
	if (0 != (event & XIICPS_EVENT_COMPLETE_RECV))
	{
		//LOG_INFO("I2C RX");
		totalErrorCount = 0;
		recvComplete = true;
	}
	else if (0 != (event & XIICPS_EVENT_COMPLETE_SEND))
	{
		//LOG_INFO("I2C TX");
		totalErrorCount = 0;
		sendComplete = true;
	}
	else if (0 != (event & XIICPS_EVENT_ARB_LOST))
	{
		LOG_TRACE("I2C arb list");
		//arbitritrarionLost = true;
	}
	else if (0 != (event & XIICPS_EVENT_TIME_OUT))
	{
		LOG_TRACE("I2C timeout");
	}
	else if (0 != (event & XIICPS_EVENT_ERROR))
	{
		if(totalRecvErrorCount % 100 == 0)
			LOG_ERROR("I2C EV Error: ", totalRecvErrorCount);
		totalRecvErrorCount++;
		recvError = 1;
	}
	else if (0 != (event & XIICPS_EVENT_SLAVE_RDY))
	{
		LOG_TRACE("I2C ready");
	}
	else if (0 != (event & XIICPS_EVENT_RX_OVR))
	{
		LOG_TRACE("I2C rx ovr");
	}
	else if (0 != (event & XIICPS_EVENT_TX_OVR))
	{
		LOG_TRACE("I2C tx ovr");
	}
	else if (0 != (event & XIICPS_EVENT_RX_UNF))
	{
		LOG_TRACE("I2C rx unf");
	}
	else //if (0 == (event & XIICPS_EVENT_SLAVE_RDY))
	{
		totalErrorCount++;
		//TODO handle error
		LOG_TRACE("I2C unknow Error: ", totalErrorCount, " - ", event);
	}
}

#endif

I2CController::I2CController(const I2CConfig& config) : m_config(config)
{

	
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
		THROW_CONTROLLERS_EXCEPTION(services::ControllersErrorId::I2cInitializationError, "XIicPs_LookupConfig XST_FAILURE");
	}

	Status = XIicPs_CfgInitialize(&m_config.i2cPsInstance, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS)
	{
		THROW_CONTROLLERS_EXCEPTION(services::ControllersErrorId::I2cInitializationError, "XIicPs_CfgInitialize XST_FAILURE");
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XIicPs_SelfTest(&m_config.i2cPsInstance);
	if (Status != XST_SUCCESS)
	{
		THROW_CONTROLLERS_EXCEPTION(services::ControllersErrorId::I2cInitializationError, "XIicPs_SelfTest XST_FAILURE");
	}

#ifndef I2C_POLLING
	/*
	 * Connect the IIC to the interrupt subsystem such that interrupts can
	 * occur. This function is application specific.
	 */
	Status = SetupInterruptSystem(&m_config.i2cPsInstance);

	if (Status != XST_SUCCESS)
	{
		THROW_CONTROLLERS_EXCEPTION(services::ControllersErrorId::I2cInitializationError, "SetupInterruptSystem XST_FAILURE");
	}
#endif
	/*
	 * Set the IIC serial clock rate.
	 */
	XIicPs_SetSClk(&m_config.i2cPsInstance, m_config.clockRate);
}

uint8_t I2CController::readData(uint8_t slaveAddr, uint8_t registerAddr,  uint8_t *buffer, uint8_t bufferSize)
{
	int status = 0;
#ifndef I2C_POLLING
	sendComplete = FALSE;
	uint8_t printed = 0;

	//LOG_INFO("rmS");
	//XIicPs_ClearOptions(&m_config.i2cPsInstance, XIICPS_REP_START_OPTION);
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
			for (int Delay = 0; Delay < 0xFFFF; Delay++);
			XIicPs_MasterSend(&m_config.i2cPsInstance, &registerAddr, 1, slaveAddr);
			if(printed==0){printed=1;LOG_ERROR("I2C Error during sending register address");};
		}
	}
	printed = 0;
#else
	status = XIicPs_MasterSendPolled(&m_config.i2cPsInstance, &registerAddr, 1, slaveAddr);

	if (status != XST_SUCCESS)
	{
		LOG_ERROR("I2C Error during sending register address");
	}
#endif
	//LOG_TRACE("I2C sent waiting");

	/*
	 * Wait bus activities to finish.
	 */
	while (XIicPs_BusIsBusy(&m_config.i2cPsInstance))
	{
		/* NOP */
	}
	//LOG_TRACE("I2C sent ready");
	/*
	 * Receive data from slave, errors are reported through
	 * totalErrorCount.
	 */
	uint8_t bufferAux[30];
#ifndef I2C_POLLING
	//LOG_INFO("rmR");
	recvComplete = FALSE;

	//XIicPs_ClearOptions(&m_config.i2cPsInstance, XIICPS_REP_START_OPTION);    //enable sending STOP bit after data transfer
	XIicPs_MasterRecv(&m_config.i2cPsInstance, bufferAux, bufferSize, slaveAddr);


	while (!recvComplete)
	{
		if (0 != totalErrorCount)
		{
			//TODO handle error
			for (int Delay = 0; Delay < 0xFFFF; Delay++);
			if(printed == 0){printed=1; LOG_ERROR("I2C Error during receiving data register"); };
		}
		if(recvError)
		{
			recvError = 0;
			XIicPs_MasterRecv(&m_config.i2cPsInstance, bufferAux, bufferSize, slaveAddr);
		}
	}
	//LOG_TRACE("I2C received");
	/*
	 * Wait bus activities to finish.
	 */
	while (XIicPs_BusIsBusy(&m_config.i2cPsInstance))
	{
		/* NOP */
	}
#else
	status = XIicPs_MasterRecvPolled(&m_config.i2cPsInstance, bufferAux, bufferSize, slaveAddr);
	if (status != XST_SUCCESS)
	{
		LOG_ERROR("I2C Error during receiving data");
	}
	while (XIicPs_BusIsBusy(&m_config.i2cPsInstance))
	{
		/* NOP */
	}
#endif
	for(int i=0; i< bufferSize;i++)
	{
		buffer[i] = bufferAux[i];
	}
	LOG_TRACE("I2C received ready");
}

uint8_t I2CController::sendData(uint8_t slaveAddr, uint8_t *buffer, uint32_t bufferSize)
{	
#ifndef I2C_POLLING
	//LOG_INFO("wmS");
	sendComplete = FALSE;
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
	auto status = XIicPs_MasterSendPolled(&m_config.i2cPsInstance, buffer, bufferSize, slaveAddr);

	if (status != XST_SUCCESS)
	{
		LOG_ERROR("I2C Error during sending I2C data");
	}
#endif
	return XST_SUCCESS;
}
#ifndef I2C_POLLING
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
	if (NULL == IntcConfig)
	{
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(&m_config.InterruptController, IntcConfig, IntcConfig->CpuBaseAddress);

	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
				     (Xil_ExceptionHandler)XScuGic_InterruptHandler,
				     &m_config.InterruptController);


//	uint8_t priority, trigger;
//	XScuGic_GetPriorityTriggerType(&m_config.InterruptController, m_i2cIntVectorId, &priority, &trigger);
//
//	priority = 0;
//	XScuGic_SetPriorityTriggerType(&m_config.InterruptController, m_i2cIntVectorId, priority, trigger);
//
//	priority = 0;
//	trigger  = 0;
//	XScuGic_GetPriorityTriggerType(&m_config.InterruptController, m_i2cIntVectorId, &priority, &trigger);

	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	Status = XScuGic_Connect(&m_config.InterruptController, m_i2cIntVectorId,
				 (Xil_InterruptHandler)XIicPs_MasterInterruptHandler,
				 (void *)IicPsPtr);
	if (Status != XST_SUCCESS)
	{
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

	/*
	 * Setup the handlers for the IIC that will be called from the
	 * interrupt context when data has been sent and received, specify a
	 * pointer to the IIC driver instance as the callback reference so
	 * the handlers are able to access the instance data.
	 */
	XIicPs_SetStatusHandler(&m_config.i2cPsInstance, (void *) &m_config.i2cPsInstance, irqHandler);
	return XST_SUCCESS;
}
#endif
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
