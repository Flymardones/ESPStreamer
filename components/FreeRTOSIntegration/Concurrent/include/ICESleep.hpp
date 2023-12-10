/*
   ICESleep.hpp
   Author: KNE
*/

#ifndef FREERTOSINTEGRATION_CONCURRENT_THREADING_INCLUDE_ICESLEEP_HPP_
#define FREERTOSINTEGRATION_CONCURRENT_THREADING_INCLUDE_ICESLEEP_HPP_

#ifdef __cplusplus
extern "C" {
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
	void ICESleep(int iMilliseconds);
#ifdef __cplusplus
}
#endif

#endif /* FREERTOSINTEGRATION_CONCURRENT_THREADING_INCLUDE_ICESLEEP_HPP_ */
