#pragma once

#include <FreeRTOS.h>
#include "semphr.h"
#include "Definitions.h"
namespace RTOS
{
class QueueHandler
{
private:
	QueueHandle_t queue;
public:

	QueueHandler();
	virtual ~QueueHandler();

	void createQueue();
	void deleteQueue();
	void resetQueue();
	void removeQueue();
	cstring getName() const;
	void sendToBack(const void * itemToQueue);
	void sendToBack(const void * itemToQueue, uint32_t timeout);	
	void sendToBackOverwrite(const void * itemToQueue);
	void sendToFront(const void * itemToQueue);
	void sendToFront(const void * itemToQueue, uint32_t timeout);

	void receive(void *rxBuffer);
	void receive(void *rxBuffer, uint32_t timeout);

	void peek(void *rxBuffer);
	void peek(void *rxBuffer, uint32_t timeout);
	uint32_t getStoredMsg() const;
	uint32_t getAvailableSpace() const;
};
}