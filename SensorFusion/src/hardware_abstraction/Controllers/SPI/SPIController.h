#pragma once
#include <hardware_abstraction/Controllers/IController.h>
#include <hardware_abstraction/Controllers/SPI/SPIConfig.h>
#include <cstdint>


namespace hardware_abstraction
{
namespace Controllers
{
class SPIController : public IController
{
private:
	XSpi*        xspi;
	XSpi_Config  xspiConfig;
	uint32_t     gpioAddr;
public:
	SPIController(const SPIConfig& xspiCfg);
	virtual ~SPIController();

	void initialize() override;
	void finish();
	void writeCommand(uint8_t cmd);
	void writeData(uint8_t *pCmd, int nCmd, uint8_t *pData,int nData);
	uint8_t readRegister(uint8_t regAddr);
	void readData(uint8_t start_addr, uint8_t *data, uint32_t nData);
	bool selfTest() override;
};
}
}
