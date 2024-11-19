 #pragma once
#include <iostream>
#include <exception>
#include "BaseException.h"

namespace services
{

enum ServicesErrorId
{
    JsonFileParsingError = ErrorIdOffset::ServicesLayer,
	JsonNodeParsingError,
	LoggerUnknownLevel,
	RtosTaskCreationError,
	NullptrObserverCallback,
	NullptrObserver,
	UnregisteredObserver,
	DynamicCastNullptr
};

class ServicesException : public BaseException
{
public:
	ServicesException(const uint32_t errorID, const std::string& errorMsg) : BaseException(errorID, errorMsg) {};
    virtual ~ServicesException() = default;
};
}
