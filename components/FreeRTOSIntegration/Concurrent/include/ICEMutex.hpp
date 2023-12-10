/*
   ICEMutex.hpp
   Author: KNE
   A simple FreeRTOS mutex as c++ object.
*/

#ifndef NETWORK_AVTP_INCLUDE_ICEMUTEX_HPP_
#define NETWORK_AVTP_INCLUDE_ICEMUTEX_HPP_

extern "C" {
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
}

class ICEMutex {
private:
	StaticSemaphore_t kAVTPBufferMutex;
	SemaphoreHandle_t kAVTPBufferMutexHandle;

public:
	ICEMutex();
	~ICEMutex();
	int lock(int timeout);
	int unlock();
	TaskHandle_t GetMutexHolder();

};

#endif /* NETWORK_AVTP_INCLUDE_ICEMUTEX_HPP_ */
