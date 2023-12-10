#include "ICEQueue.hpp"

// Empty ICEQueue constructor for the Static queue
ICEQueue::ICEQueue()
{
	this->isQueueValid = false;
	this->length = 0;
}

ICEQueue::ICEQueue(int length, int size_of_element)
{
	queue = xQueueCreate(length, size_of_element);
	this->length = length;
	
	if( queue == NULL )
	{
		isQueueValid = false;
	}
	else
	{
		isQueueValid = true;
	}
}

ICEQueueStatic::ICEQueueStatic( int maxElements, int elementSize, uint8_t * queueStorageBuffer )
{
	queue = xQueueCreateStatic(maxElements, elementSize, queueStorageBuffer, &_staticQueue);
	if( queue == NULL )
	{
		isQueueValid = false;
	}
	else
	{
		isQueueValid = true;
		this->length = maxElements; 
	}
}

ICEQueue::~ICEQueue() {
	this->Delete();
}

void ICEQueue::Delete()
{
	vQueueDelete( this->queue );
	this->length = 0;
	this->isQueueValid = false;
	this->queue = NULL;
}

bool ICEQueue::sendToQueue(void * object, int msMaxTimeout)
{
	if(isQueueValid)
	{
		if(!xQueueSend(queue, object, msMaxTimeout/portTICK_PERIOD_MS)) {
			return false;		// Failed to send to queue
		} else {
			return true;		// Succeeded to send to queue
		};
	}
	else
		return false;			// The queue is not valid
}

bool ICEQueue::receiveFromQueue(void * buffer, int msMaxTimeout)
{
	if(isQueueValid && (messagesWaiting() != 0))
	{
		if(xQueueReceive(queue, buffer, msMaxTimeout/portTICK_PERIOD_MS))
		{
			return true;		//Successfully retrieved object from queue
		}
		else
		{
			return false;		// Failed to retrieve object from queue
		}
	}
	else
		return false;			// The queue is not valid
}

int ICEQueue::messagesWaiting()
{
	if( isQueueValid )
	{
		return uxQueueMessagesWaiting(queue);
	}
	return 0;
}

int ICEQueue::spacesAvailable()
{
    if( isQueueValid )
    {
        return uxQueueSpacesAvailable( queue );
    }
    return 0;
}