
#include <business_logic/Osal/StreamBufferHandler.h>

namespace business_logic
{
namespace Osal
{
StreamBufferHandler::StreamBufferHandler(size_t bufferSize, size_t trgLvl) : size(bufferSize), triggerLevel(trgLvl)
{

}

StreamBufferHandler::~StreamBufferHandler()
{

}

void StreamBufferHandler::create()
{
    buffer = xStreamBufferCreate(size, triggerLevel);

    if( buffer == NULL )
    {
        /* Queue was not created and must not be used. */
    }
}

void StreamBufferHandler::remove()
{
	vStreamBufferDelete(buffer);
}

bool StreamBufferHandler::reset()
{
	return xStreamBufferReset(buffer);
}


void StreamBufferHandler::send(const void * txBuffer, size_t dataSize)
{
	const auto bytesSent = xStreamBufferSend( buffer, txBuffer, dataSize, pdMS_TO_TICKS(0));
	if(bytesSent != dataSize)
	{
		
	}
}

void StreamBufferHandler::send(const void * txBuffer, size_t dataSize, uint32_t timeout)
{
	const auto bytesSent = xStreamBufferSend( buffer, txBuffer, dataSize, pdMS_TO_TICKS(timeout));
	if(bytesSent != dataSize)
	{
		
	}
}

void StreamBufferHandler::receive(void *rxBuffer, size_t rxBufferSize)
{
	if(xStreamBufferReceive( buffer, rxBuffer, rxBufferSize ,static_cast<TickType_t>(0) ) == pdPASS )
	{
		/* xRxedStructure now contains a copy of xMessage. */
	}
}

void StreamBufferHandler::receive(void *rxBuffer, size_t dataSize, uint32_t timeout)
{
	if(xStreamBufferReceive( buffer, rxBuffer, dataSize, static_cast<TickType_t>(timeout) ) == pdPASS )
	{
		/* xRxedStructure now contains a copy of xMessage. */
	}
}


size_t StreamBufferHandler::getAvailableBytes() const 
{
	return static_cast<size_t>(xStreamBufferBytesAvailable(buffer));
}

size_t StreamBufferHandler::getAvailableSpace() const	 
{
	return static_cast<size_t>(xStreamBufferSpacesAvailable(buffer));
}
	
bool StreamBufferHandler::isEmpty() const	 
{
	return static_cast<bool>(xStreamBufferIsEmpty(buffer));
}

bool StreamBufferHandler::isFull() const	 
{
	return static_cast<bool>(xStreamBufferIsFull(buffer));
}	

bool StreamBufferHandler::setTriggerLevel(size_t level)
{
	triggerLevel = level;
	return static_cast<bool>(xStreamBufferSetTriggerLevel(buffer,triggerLevel));
}	
}
}
