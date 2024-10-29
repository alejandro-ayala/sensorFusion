#include <business_logic/Osal/QueueHandler.h>

namespace business_logic
{
namespace Osal
{
QueueHandler::QueueHandler()
{

}

QueueHandler::~QueueHandler()
{

}

void QueueHandler::createQueue()
{
    queue = xQueueCreate( 10, sizeof( struct AMessage * ) );

    if( queue == NULL )
    {
        /* Queue was not created and must not be used. */
    }
}

void QueueHandler::deleteQueue()
{
	vQueueDelete(queue);
}

void QueueHandler::resetQueue()
{
	xQueueReset(queue);
}

void QueueHandler::removeQueue()
{

}


cstring QueueHandler::getName() const
{
	return pcQueueGetName(queue);
}

void QueueHandler::sendToBack(const void * itemToQueue)
{
	if( xQueueSendToBack( queue, itemToQueue, static_cast<TickType_t>(0) ) != pdPASS )
	{
		/* Failed to post the message, even after 10 ticks. */
	}
}

void QueueHandler::sendToBack(const void * itemToQueue, uint32_t timeout)
{
	if( xQueueSendToBack( queue, itemToQueue, static_cast<TickType_t>(timeout) ) != pdPASS )
	{
		/* Failed to post the message, even after 10 ticks. */
	}
}

void QueueHandler::sendToBackOverwrite(const void * itemToQueue)
{
	xQueueOverwrite(queue, itemToQueue);
}

void QueueHandler::sendToFront(const void * itemToQueue)
{
	xQueueSendToFront(queue, itemToQueue, static_cast<TickType_t>(0));
}

void QueueHandler::sendToFront(const void * itemToQueue, uint32_t timeout)
{
	xQueueSendToFront( queue, itemToQueue, static_cast<TickType_t>(timeout));
}
	
void QueueHandler::receive(void *rxBuffer)	
{
	if(xQueueReceive( queue, rxBuffer,static_cast<TickType_t>(0) ) == pdPASS )
	{
		/* xRxedStructure now contains a copy of xMessage. */
	}
}

void QueueHandler::receive(void *rxBuffer, uint32_t timeout)	
{
	if(xQueueReceive( queue, rxBuffer,static_cast<TickType_t>(timeout) ) == pdPASS )
	{
		/* xRxedStructure now contains a copy of xMessage. */
	}
}

void QueueHandler::peek(void *rxBuffer)
{
	if( xQueuePeek( queue, rxBuffer, static_cast<TickType_t>(0)) )
	{
		// pcRxedMessage now points to the struct AMessage variable posted
		// by vATask, but the item still remains on the queue.
	}
}

void QueueHandler::peek(void *rxBuffer, uint32_t timeout)
{
	if( xQueuePeek( queue, rxBuffer, static_cast<TickType_t>(timeout)) )
	{
		// pcRxedMessage now points to the struct AMessage variable posted
		// by vATask, but the item still remains on the queue.
	}
}

uint32_t QueueHandler::getStoredMsg() const
{
	return static_cast<uint32_t>(uxQueueMessagesWaiting(queue));
}

uint32_t QueueHandler::getAvailableSpace() const
{
	return static_cast<uint32_t>(uxQueueSpacesAvailable(queue));
}
	
}
}
