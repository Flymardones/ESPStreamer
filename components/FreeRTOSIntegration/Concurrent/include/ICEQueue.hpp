#ifndef CONCURRENT_INCLUDE_ICEQUEUE_HPP_
#define CONCURRENT_INCLUDE_ICEQUEUE_HPP_

extern "C" {
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
}

class ICEQueue {
protected:
	QueueHandle_t queue;
	bool isQueueValid;
	int length;

public:
	ICEQueue();
	ICEQueue(int length, int size_of_element);
	~ICEQueue();
	void Delete();
	bool sendToQueue(void * object, int msMaxTimeout = 10);
	bool receiveFromQueue(void * buffer, int msMaxTimeout = 100);
	int messagesWaiting();
	int spacesAvailable();
};


class ICEQueueStatic : public ICEQueue {
public:
    ICEQueueStatic( int maxElements, int elementSize, uint8_t * queueStorageBuffer );
protected:
    StaticQueue_t _staticQueue;
};


#endif /* CONCURRENT_INCLUDE_ICEQUEUE_HPP_ */