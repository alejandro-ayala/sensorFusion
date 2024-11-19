#pragma once
#include <services/Exception/BusinessLogicException.h>
#include "ControllersException.h"
#include "DevicesException.h"
#include "ServicesException.h"


#define DEVICES_ASSERT(condition, exceptionId, msg) if(!(condition)) {throw services::DevicesException(exceptionId, std::string(msg)); std::terminate();}
#define BUSINESS_LOGIC_ASSERT(condition, exceptionId, msg) if(!(condition)) {throw services::BusinessLogicException(exceptionId, std::string(msg)); std::terminate();}
#define SERVICES_ASSERT(condition, exceptionId, msg) if(!(condition)) {throw services::ServicesException(exceptionId, std::string(msg)); std::terminate();}


#define THROW_CONTROLLERS_EXCEPTION(exceptionId, msg) throw services::ControllersException(exceptionId, std::string(msg))
#define THROW_DEVICES_EXCEPTION(exceptionId, msg) throw services::DevicesException(exceptionId, std::string(msg))
#define THROW_BUSINESS_LOGIC_EXCEPTION(exceptionId, msg) throw services::BusinessLogicException(exceptionId, std::string(msg))
#define THROW_SERVICES_EXCEPTION(exceptionId, msg) throw services::ServicesException(exceptionId, std::string(msg))

