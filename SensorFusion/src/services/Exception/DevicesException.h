 #pragma once
#include <iostream>
#include <exception>
#include "BaseException.h"

namespace services
{

enum DevicesErrorId
{
    LidarInitializationError = ErrorIdOffset::DevicesLayer,
};

class DevicesException : public BaseException
{
public:
	DevicesException(const uint32_t errorID, const std::string& errorMsg) : BaseException(errorID, errorMsg) {};
    virtual ~DevicesException() = default;
};
}
