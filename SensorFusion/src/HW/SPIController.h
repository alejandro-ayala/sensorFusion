#pragma once
#include <cstdint>
#include "xspi.h"
#include "IController.h"

namespace Hardware
{
class SPIController : public IController
{
private:
	XSpi*        xspi;
	XSpi_Config  xspiConfig;
	uint32_t     gpioAddr;
public:
	SPIController();
	virtual ~SPIController();

	void initialize() override;
	void finish();
	void writeCommand(uint8_t cmd);
	void writeData(uint8_t *pCmd, int nCmd, uint8_t *pData,int nData);
	void selfTest();
};
}

