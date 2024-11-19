 #pragma once
#include <iostream>
#include <exception>
#include "BaseException.h"
namespace services
{

enum BusinessLogicErrorId
{
    TaskCreationError = ErrorIdOffset::BusinessLogicLayer,
	DynamicCastError,
	QueueAllocationError,
	QueueIsFull,
	CameraTimeout,
	MemoryAllocationFailed,
	InvalidArgument,
	SerializationError
};

class BusinessLogicException : public BaseException
{
public:
	BusinessLogicException(const uint32_t errorID, const std::string& errorMsg) : BaseException(errorID, errorMsg) {};
    virtual ~BusinessLogicException() = default;
};
}
