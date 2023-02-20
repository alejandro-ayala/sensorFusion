#include <FreeRTOS.h>
#include <message_buffer.h>
#include <stream_buffer.h>
#include "MessagesBufferHandler.h"

namespace RTOS
{
MessagesBufferHandler::MessagesBufferHandler(size_t bufferSize) : size(bufferSize)
{

}

MessagesBufferHandler::~MessagesBufferHandler()
{

}

void MessagesBufferHandler::create()
{
    buffer = xMessageBufferCreate(size);

    if( buffer == NULL )
    {
        /* Queue was not created and must not be used. */
    }
}

void MessagesBufferHandler::remove()
{
	vMessageBufferDelete(buffer);
}

bool MessagesBufferHandler::reset()
{
	return xMessageBufferReset(buffer);
}


void MessagesBufferHandler::send(const void * txBuffer, size_t dataSize)
{
	const auto bytesSent = xMessageBufferSend( buffer, txBuffer, dataSize, pdMS_TO_TICKS(0));
	if(bytesSent != dataSize)
	{
		
	}
}

void MessagesBufferHandler::send(const void * txBuffer, size_t dataSize, uint32_t timeout)
{
	const auto bytesSent = xMessageBufferSend( buffer, txBuffer, dataSize, pdMS_TO_TICKS(timeout));
	if(bytesSent != dataSize)
	{
		
	}
}

void MessagesBufferHandler::receive(void *rxBuffer, size_t msgSize)
{
	if(xMessageBufferReceive(buffer, rxBuffer, msgSize, static_cast<TickType_t>(0) ) == pdPASS )
	{
		/* xRxedStructure now contains a copy of xMessage. */
	}
}

void MessagesBufferHandler::receive(void *rxBuffer, size_t msgSize, uint32_t timeout)
{
	if(xMessageBufferReceive(buffer, rxBuffer, msgSize, static_cast<TickType_t>(timeout) ) == pdPASS )
	{
		/* xRxedStructure now contains a copy of xMessage. */
	}
}

size_t MessagesBufferHandler::getAvailableSpace() const	 
{
	return static_cast<size_t>(xMessageBufferSpacesAvailable(buffer));
}
	
bool MessagesBufferHandler::isEmpty() const	 
{
	return static_cast<bool>(xMessageBufferIsEmpty(buffer));
}

bool MessagesBufferHandler::isFull() const	 
{
	return static_cast<bool>(xMessageBufferIsFull(buffer));
}
}
