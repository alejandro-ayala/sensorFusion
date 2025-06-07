#pragma once

#include <FreeRTOS.h>
#include "semphr.h"
#include "Definitions.h"
#include "business_logic/ImageCapturer3D/LidarPoint.h"
namespace business_logic
{
namespace Osal
{
class QueueHandler
{
private:
	QueueHandle_t queue;
	uint32_t m_queueLength;
	uint32_t m_itemSize;

public:

	QueueHandler(uint32_t queuelength, uint32_t itemSize);
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

	void receive(void* rxBuffer);
	void receive(void *rxBuffer, uint32_t timeout);

	void peek(void *rxBuffer);
	void peek(void *rxBuffer, uint32_t timeout);
	uint32_t getStoredMsg() const;
	uint32_t getAvailableSpace() const;
};
}
}
