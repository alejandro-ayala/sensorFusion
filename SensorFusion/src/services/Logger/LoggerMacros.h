#pragma once
#include "services/Logger/Logger.h"

#define LOG_TRACE(...) services::Logger::getInstance().log(services::LogLevel::Trace, __VA_ARGS__)
#define LOG_DEBUG(...) services::Logger::getInstance().log(services::LogLevel::Debug, __VA_ARGS__)
#define LOG_INFO(...)  services::Logger::getInstance().log(services::LogLevel::Info, __VA_ARGS__)
#define LOG_WARNING(...) services::Logger::getInstance().log(services::LogLevel::Warn, __VA_ARGS__)
#define LOG_ERROR(...) services::Logger::getInstance().log(services::LogLevel::Error, __VA_ARGS__)
#define LOG_FATAL(...) services::Logger::getInstance().log(services::LogLevel::Critical, __VA_ARGS__)

