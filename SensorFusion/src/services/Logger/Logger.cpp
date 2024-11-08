#include "Logger.h"
#include "stdio.h"
//#include "services/Exception/SystemExceptions.h"

namespace services
{

Logger& Logger::getInstance()
{
	static Logger instance;
	return instance;
}

Logger::Logger() : ILogger(), m_logLevel(LogLevel::Debug), m_disable(false)
{
	initialize();
}

void Logger::initialize()
{

}

void Logger::log(LogLevel logLevel, const std::string& msg)
{
	if((m_logLevel > logLevel) || m_disable) 
    return;

	std::string logMsg;
    switch (logLevel)
    {
     	 case LogLevel::Trace: logMsg = "[TRACE] " + msg; break;
     	 case LogLevel::Debug: logMsg = "[DEBUG] " + msg; break;
     	 case LogLevel::Info: logMsg = "[INFO] " + msg; break;
     	 case LogLevel::Warn: logMsg = "[WARNING] " + msg; break;
     	 case LogLevel::Error: logMsg = "[ERROR] " + msg; break;
     	 case LogLevel::Critical: logMsg = "[CRITICAL] " + msg; break;
     	 default: break;//THROW_SERVICES_EXCEPTION(ServicesErrorId::LoggerUnknownLevel, "Unknown logging level")
    }
  std::cout << logMsg << std::endl;
}

void Logger::setLogLevel(LogLevel logLevel)
{
	m_logLevel = logLevel;
}

void Logger::disable()
{
	m_disable = true;
}

void Logger::enable()
{
	m_disable = false;
}

} //namespace services
