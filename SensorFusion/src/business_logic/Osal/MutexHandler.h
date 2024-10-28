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

template<class Mutex>
class lock_guard
{
private:
	Mutex& _m;
	cstring _name;
public:
	//lock_guard(Mutex& m): _m(m), _name((cstring)'\0') {_m.lock();}
	lock_guard(Mutex& m, cstring name): _m(m), _name(name) {PRINT_DEBUG1(_name," lock"); _m.lock(); PRINT_DEBUG1(_name," locked");}
	lock_guard(Mutex& m, cstring name,uint16_t timeout): _m(m), _name(name)
	{
		_m.timeout = timeout;
		PRINT_DEBUG1(_name," trylock");
		if(_m.try_lock())
		{
			PRINT_DEBUG1(_name," locked");
		}
		else
		{
			PRINT_DEBUG1(_name," Mutex is locked from long time. Releasing");
		}
	}
	virtual ~lock_guard() { PRINT_DEBUG1(_name," unlock"); _m.unlock(); PRINT_DEBUG1(_name," unlocked");}

	lock_guard(const lock_guard&) = delete;
	lock_guard& operator=(const lock_guard&) = delete;
};

}
}
