 #pragma once
#include <iostream>
#include <exception>
#include "BaseException.h"
namespace services
{

enum ControllersErrorId
{
    AdcInitializationError = ErrorIdOffset::ControllersLayer,
	CanInitializationError,
	GpioInitializationError,
	PwmInitializationError,
	PwmStartError,
	PwmStopError,
	PwmChannelConfiguration,
	SpiInitializationError,
	TimerInitializationError,
	TimerStartError,
	TimerStopError,
	UartInitializationError,
	UartTxError,
	UartRxError,
	I2cInitializationError,
};

class ControllersException : public BaseException
{
public:
	ControllersException(const uint32_t errorID, const std::string& errorMsg) : BaseException(errorID, errorMsg) {};
    virtual ~ControllersException() = default;
};
}
