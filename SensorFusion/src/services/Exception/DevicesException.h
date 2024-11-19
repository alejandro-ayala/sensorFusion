 #pragma once
#include <iostream>
#include <exception>
#include "BaseException.h"

namespace services
{

enum DevicesErrorId
{
    Esp32InitializationError = ErrorIdOffset::DevicesLayer,
	Esp32ParsingMacError,
	Esp32SwResetError,
	Esp32AdvertisingError,
	Esp32WaitingConnectionError,
	Esp32ConfigureSppError,
	ElectrodeInitializationError
};

class DevicesException : public BaseException
{
public:
	DevicesException(const uint32_t errorID, const std::string& errorMsg) : BaseException(errorID, errorMsg) {};
    virtual ~DevicesException() = default;
};
}
