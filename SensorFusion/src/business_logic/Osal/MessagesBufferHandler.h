#pragma once

#include <FreeRTOS.h>
#include <message_buffer.h>
#include <stream_buffer.h>

namespace business_logic
{
namespace Osal
{
class MessagesBufferHandler
{
private:
	StreamBufferHandle_t  buffer;
	size_t size;
public:

	MessagesBufferHandler(size_t bufferSize);
	virtual ~MessagesBufferHandler();

	void create();
	void remove();
	bool reset();

	void send(const void * txBuffer, size_t dataSize);
	void send(const void * txBuffer, size_t dataSize, uint32_t timeout);
	void receive(void *rxBuffer, size_t msgSize);
	void receive(void *rxBuffer, size_t msgSize, uint32_t timeout);


	uint32_t getStoredMsg() const;
	size_t   getAvailableBytes() const;
	size_t   getAvailableSpace() const;
	bool isEmpty() const;
	bool isFull() const;
	
};
}
}
