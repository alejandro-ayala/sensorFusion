#pragma once

#include <FreeRTOS.h>
#include "semphr.h"
//#include <functional>
#include <string>

namespace business_logic
{
namespace Osal
{
using TaskFunction = void (*)(void*);
using VoidPtr = void*;

class TaskHandler
{
private:
	TaskHandle_t m_task;
	static const uint32_t StackSize{512};
	std::string m_taskName;

public:

	TaskHandler(TaskFunction taskFunction, const char* taskName);
	TaskHandler(TaskFunction taskFunction, const char* taskName, uint32_t priority);
	TaskHandler(TaskFunction taskFunction, const char* taskName, uint32_t priority, VoidPtr taskParam, uint32_t stackSize = configMINIMAL_STACK_SIZE);
	virtual ~TaskHandler();

	void deleteMe() const;
	void delayUntil(const uint32_t milliSecond) const;
	void delay(const uint32_t milliSecond) const;
	void resume() const;
	void suspend() const;
	void setPriority(uint32_t newPriority) const;
	uint32_t getPriority() const;
	TaskHandle_t getTaskHandler() const;
};
}
}
