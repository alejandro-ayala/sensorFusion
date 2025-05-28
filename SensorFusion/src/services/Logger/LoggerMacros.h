#pragma once
#include "services/Logger/Logger.h"

#define LOG_IF_ENABLED(level, ...) \
    do { \
        if (services::Logger::getInstance().isLevelEnabled(level)) \
            services::Logger::getInstance().log(level, __VA_ARGS__); \
    } while (0)

#define LOG_TRACE(...) LOG_IF_ENABLED(services::LogLevel::Trace, __VA_ARGS__)
#define LOG_DEBUG(...) LOG_IF_ENABLED(services::LogLevel::Debug, __VA_ARGS__)
#define LOG_INFO(...)  LOG_IF_ENABLED(services::LogLevel::Info,  __VA_ARGS__)
#define LOG_WARNING(...) LOG_IF_ENABLED(services::LogLevel::Warn, __VA_ARGS__)
#define LOG_ERROR(...) LOG_IF_ENABLED(services::LogLevel::Error, __VA_ARGS__)
#define LOG_FATAL(...) LOG_IF_ENABLED(services::LogLevel::Critical, __VA_ARGS__)

#define logMemoryUsage() /*{\
	const std::string freeHeapMsg = std::string(__FUNCTION__) + "::" + std::to_string(__LINE__) + " -- Heap state:" +  std::to_string(xPortGetFreeHeapSize()) + " -- minMark " + std::to_string(xPortGetMinimumEverFreeHeapSize()) + " stackSize: " + std::to_string(uxTaskGetStackHighWaterMark( NULL ));\
	LOG_DEBUG(freeHeapMsg);\
           }*/
