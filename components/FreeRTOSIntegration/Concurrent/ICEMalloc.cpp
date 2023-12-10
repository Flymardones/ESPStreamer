/*
   ICEMalloc.hpp
   Author: KNE
   Allocates memory dynamically within FreeRTOS
*/

#include "ICEMalloc.hpp"

void* ICEMalloc(uint32_t size) {
	void* memory = pvPortMalloc(size);
	return memory;
};

int ICEFree(void* pointer) {
	if (pointer != NULL) {
		vPortFree(pointer);
		return 1;
	} else {
		return 0;
	}
}

void *operator new(std::size_t size) {
    void *out = ::ICEMalloc(size);
    #ifdef __EXCEPTIONS
        if (!out) throw "Error in ICEMalloc!";
    #else
        if (!out) abort();
    #endif
    return out;
}

void operator delete(void* pVoidPointer) noexcept (true) {
    ICEFree(pVoidPointer);
}