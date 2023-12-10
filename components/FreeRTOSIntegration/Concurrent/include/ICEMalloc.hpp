/*
   ICEMalloc.hpp
   Author: KNE
   Allocates memory dynamically within FreeRTOS
*/

#ifndef CONCURRENT_INCLUDE_ICEMALLOC_HPP_
#define CONCURRENT_INCLUDE_ICEMALLOC_HPP_

extern "C" {
#include "freertos/FreeRTOS.h"
}
#include <cstdlib>

void* ICEMalloc(uint32_t size);
int ICEFree(void* pointer);

void *operator new(std::size_t size);
void operator delete(void* pVoidPointer) noexcept (true);

#endif /* CONCURRENT_INCLUDE_ICEMALLOC_HPP_ */
