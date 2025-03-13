#include <business_logic/Osal/QueueHandler.h>
#include "services/Exception/SystemExceptions.h"
#include "services/Logger/LoggerMacros.h"
#include "business_logic/ImageCapturer3D/LidarPoint.h"
namespace business_logic
{
namespace Osal
{
QueueHandler::QueueHandler(uint32_t queuelength, uint32_t itemSize) : m_queueLength(queuelength), m_itemSize(itemSize)
{
	try
	{
		createQueue();
	}
	catch (const services::BaseException& e)
	{
		THROW_BUSINESS_LOGIC_EXCEPTION( services::BusinessLogicErrorId::QueueAllocationError,  e.what());
	}

}

QueueHandler::~QueueHandler()
{
	deleteQueue();
}

void QueueHandler::createQueue()
{
    queue = xQueueCreate( m_queueLength, m_itemSize );

    if( queue == NULL )
    {
    	THROW_BUSINESS_LOGIC_EXCEPTION(services::BusinessLogicErrorId::QueueAllocationError, "Error creating queue for Image3D captures");
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
	LOG_TRACE("Sending pointer to queue: %p", itemToQueue);
	if( xQueueSendToBack( queue, itemToQueue, static_cast<TickType_t>(0) ) != pdPASS )
	{
		THROW_BUSINESS_LOGIC_EXCEPTION(services::BusinessLogicErrorId::QueueIsFull, "Failed to insert capture in queue");
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
	
void QueueHandler::receive(void* rxBuffer)
{
    xQueueReceive(queue, rxBuffer, (TickType_t)10);
    LOG_TRACE("Receiving from queue into buffer: %p", rxBuffer);
    return;
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
