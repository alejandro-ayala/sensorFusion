#include <business_logic/Osal/MutexHandler.h>

namespace business_logic
{
namespace Osal
{
MutexHandler::MutexHandler() : timeout(1)
{
	mutex = xSemaphoreCreateBinary();//xSemaphoreCreateMutex();
	 xSemaphoreGive(mutex);
}

MutexHandler::MutexHandler(uint32_t timeout) : timeout(timeout)
{
	mutex = xSemaphoreCreateBinary();//xSemaphoreCreateMutex();
	 xSemaphoreGive(mutex);
}

MutexHandler::~MutexHandler()
{
	vSemaphoreDelete(mutex);
}


void MutexHandler::lock()
{
	//ASSERT_FUNC(ERROR_CODE::NULL_PARAMETER, mutex != nullptr, "SPD mutex is null");
	xSemaphoreTake(mutex, portMAX_DELAY);
}

bool MutexHandler::try_lock()
{
	//ASSERT_FUNC(ERROR_CODE::NULL_PARAMETER, mutex != nullptr, "SPD mutex is null");
	if(xSemaphoreTake(mutex, pdMS_TO_TICKS(timeout*1000)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void MutexHandler::unlock()
{
	//ASSERT_FUNC(ERROR_CODE::NULL_PARAMETER, mutex != nullptr, "SPD mutex is null");
	xSemaphoreGive(mutex);
}

RecursiveMutexHandler::RecursiveMutexHandler() : timeout(1)
{
	mutex = xSemaphoreCreateRecursiveMutex();
}

RecursiveMutexHandler::RecursiveMutexHandler(uint32_t timeout) : timeout(timeout)
{
	mutex = xSemaphoreCreateRecursiveMutex();
}

RecursiveMutexHandler::~RecursiveMutexHandler()
{
	vSemaphoreDelete(mutex);
}


void RecursiveMutexHandler::lock()
{
	//ASSERT_FUNC(ERROR_CODE::NULL_PARAMETER, mutex != nullptr, "SPD mutex is null");
	xSemaphoreTakeRecursive(mutex, portMAX_DELAY);
}

bool RecursiveMutexHandler::try_lock()
{
	//ASSERT_FUNC(ERROR_CODE::NULL_PARAMETER, mutex != nullptr, "SPD mutex is null");
	if(xSemaphoreTakeRecursive(mutex, pdMS_TO_TICKS(timeout*1000)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void RecursiveMutexHandler::unlock()
{
	//ASSERT_FUNC(ERROR_CODE::NULL_PARAMETER, mutex != nullptr, "SPD mutex is null");
	xSemaphoreGiveRecursive(mutex);
}
}
}
