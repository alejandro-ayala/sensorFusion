#include <hardware_abstraction/Controllers/SPI/SPIController.h>
#include "services/Exception/SystemExceptions.h"

namespace hardware_abstraction
{
namespace Controllers
{
SPIController::SPIController(const SPIConfig& xspiCfg): IController("SPIController")
{
	xspi = new XSpi();

	xspiConfig = xspiCfg.spiConfiguration;//{0,0,1,0,1,8,0,0,0,0,0};
	gpioAddr = xspiCfg.gpioAddress;//XPAR_PMODOLEDRGB_0_AXI_LITE_GPIO_BASEADDR;
	xspiConfig.BaseAddress = xspiCfg.spiBaseAddress;//XPAR_PMODOLEDRGB_0_AXI_LITE_SPI_BASEADDR;
}

SPIController::~SPIController()
{
	delete xspi;
}

void SPIController::initialize()
{
	int Status;

	Status = XSpi_CfgInitialize(xspi, &xspiConfig, xspiConfig.BaseAddress);
	if (Status != XST_SUCCESS)
	{
		THROW_CONTROLLERS_EXCEPTION(services::ControllersErrorId::SpiInitializationError, "Exception during XSpi_CfgInitialize");
	}

	/*
	* Set the Spi device as a master and in loopback mode.
	*/
	Status = XSpi_SetOptions(xspi, (XSP_MASTER_OPTION) & ~XSP_MANUAL_SSELECT_OPTION);
		 // Might need to be OR'ed with 0x4 for phase also

	if (Status != XST_SUCCESS)
	{
		THROW_CONTROLLERS_EXCEPTION(services::ControllersErrorId::SpiInitializationError, "Exception during XSpi_SetOptions");
	}

	Status = XSpi_SetSlaveSelect(xspi, 1);
	if (Status != XST_SUCCESS)
	{
		THROW_CONTROLLERS_EXCEPTION(services::ControllersErrorId::SpiInitializationError, "Exception during XSpi_SetSlaveSelect");
	}

	// Start the SPI driver so that the device is enabled.
	XSpi_Start(xspi);
	// Disable Global interrupt to use polled mode operation
	XSpi_IntrGlobalDisable(xspi);
	//return XST_SUCCESS;
}

void SPIController::finish()
{
	XSpi_Stop(xspi);
}

void SPIController::writeCommand(uint8_t cmd)
{
	XSpi_Transfer(xspi, &cmd, NULL, 1);
}

void SPIController::writeData(uint8_t *pCmd, int nCmd, uint8_t *pData, int nData)
{
	XSpi_Transfer(xspi, pCmd, 0, nCmd);
	if (nData != 0)
	{
		Xil_Out32(gpioAddr, 0xF); // 0b1111
		XSpi_Transfer(xspi, pData, 0, nData);
		Xil_Out32(gpioAddr, 0xE); // 0b1110
	}
}

uint8_t SPIController::readRegister(uint8_t regAddr)
{
	u8 buf[2] = {regAddr, 0x00};
	XSpi_Transfer(xspi,buf, buf, 2);
	return buf[1];
}

void SPIController::readData(uint8_t start_addr, uint8_t *data, uint32_t nData)
{
	uint8_t buf[nData + 1];
	buf[0] = start_addr;
	XSpi_Transfer(xspi, buf, buf, nData + 1);
	for (uint32_t i = 0; i < nData; i++)
	  data[i] = buf[i + 1];
}
bool SPIController::selfTest()
{
	return true;
}
}
}
