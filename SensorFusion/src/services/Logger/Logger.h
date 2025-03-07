#pragma once
#include <memory>
#include <sstream>
#include <string>
#include <iostream>
#include "ILogger.h"
#include "business_logic/Osal/MutexHandler.h"

namespace services
{

class Logger : public ILogger
{
private:
	Logger();
	Logger(const Logger&) = delete;
	void operator=(const Logger&) = delete;

	virtual void initialize();

	LogLevel m_logLevel = LogLevel::Debug;
	bool m_disable;
	std::shared_ptr<business_logic::Osal::MutexHandler> uartMutex;

	template<typename... Args>
	std::string concatenateArgsToString(const Args&... args)
	{
	    std::ostringstream oss;
	    ((oss << args << ' '), ...);
	    return oss.str();
	}
public:
	virtual ~Logger() = default;
	static Logger& getInstance();
	void setLogLevel(LogLevel) override;
	void disable() override;
	void enable() override;


	void log(LogLevel level , const std::string& msg);

	template<typename... Args>
	void log(LogLevel logLevel , const Args&... args)
	{
		if((m_logLevel > logLevel) || m_disable) return;
		const std::string msg = concatenateArgsToString(args...);
		log(logLevel, msg);
	}
};
} //namespace services
