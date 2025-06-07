#pragma once

#include <FreeRTOS.h>
#include "semphr.h"
#include "Definitions.h"

namespace business_logic
{
namespace Osal
{
class MutexHandler
{
private:
	SemaphoreHandle_t  mutex;
public:
	uint32_t           timeout;

	MutexHandler();
	MutexHandler(uint32_t timeout);
	virtual ~MutexHandler();

	MutexHandler(const MutexHandler&) = delete;
	MutexHandler& operator=(const MutexHandler&) = delete;

	void lock();
	bool try_lock();
	void unlock();
};

class RecursiveMutexHandler
{
private:
	SemaphoreHandle_t  mutex;
	uint32_t           timeout;
public:
	RecursiveMutexHandler();
	RecursiveMutexHandler(uint32_t timeout);
	virtual ~RecursiveMutexHandler();

	RecursiveMutexHandler(const RecursiveMutexHandler&) = delete;
	RecursiveMutexHandler& operator=(const RecursiveMutexHandler&) = delete;

	void lock();
	bool try_lock();
	void unlock();
};
}
}
