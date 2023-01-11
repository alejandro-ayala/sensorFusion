#pragma once

typedef struct
{
	XSpi_Config spiConfiguration;
	uint32_t    gpioAddress;
	uint32_t    spiBaseAddress;
} SPIConfig;
