#pragma once

#include <FreeRTOS.h>
#include "stream_buffer.h"

namespace business_logic
{
namespace Osal
{
class StreamBufferHandler
{
private:
	StreamBufferHandle_t  buffer;
	size_t size;
	size_t triggerLevel;
public:

	StreamBufferHandler(size_t bufferSize, size_t trgLvl);
	virtual ~StreamBufferHandler();

	void create();
	void remove();
	bool reset();

	void send(const void * txBuffer, size_t dataSize);
	void send(const void * txBuffer, size_t dataSize, uint32_t timeout);	
	void receive(void *rxBuffer, size_t dataSize);
	void receive(void *rxBuffer, size_t dataSize, uint32_t timeout);


	bool   setTriggerLevel(size_t level);
	size_t getStoredMsg() const;
	size_t getAvailableSpace() const;
	size_t getAvailableBytes() const;
	bool isEmpty() const;
	bool isFull() const;
	
};
}
}
