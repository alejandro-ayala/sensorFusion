#pragma once

namespace services
{
enum class LogLevel
{
	Trace,
	Debug,
	Info,
	Warn,
	Error,
	Critical,
	Off,

	_NumLevels
};

class ILogger
{
private:

public:
	ILogger() = default;
	virtual ~ILogger() = default;
	virtual void initialize() = 0;
	virtual void log(LogLevel, const std::string&) = 0;
	virtual void setLogLevel(LogLevel) = 0;
	virtual void disable() = 0;
	virtual void enable() = 0;
};
}
