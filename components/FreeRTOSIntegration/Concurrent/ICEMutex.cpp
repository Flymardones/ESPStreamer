/*
   ICEMutex.cpp
   Author: KNE
*/

#include "ICEMutex.hpp"

ICEMutex::ICEMutex() {
	kAVTPBufferMutexHandle = xSemaphoreCreateMutex();
}


ICEMutex::~ICEMutex() {
	vSemaphoreDelete(kAVTPBufferMutexHandle);
}


int ICEMutex::lock(int timeout) {
	int ticks = timeout / portTICK_PERIOD_MS;
	if (xSemaphoreTake(kAVTPBufferMutexHandle, ticks) != pdTRUE)
	{
		return -1;
	}

	return 0;
}


int ICEMutex::unlock() {
	if(xSemaphoreGive(kAVTPBufferMutexHandle) != pdTRUE)
	{
		return -1;
	}

	return 0;
}

TaskHandle_t ICEMutex::GetMutexHolder()
{
	return xSemaphoreGetMutexHolder(kAVTPBufferMutexHandle);
}
