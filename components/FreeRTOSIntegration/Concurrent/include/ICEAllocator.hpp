#pragma once

#include "ICEMalloc.hpp"

template <class T>
class ICEAllocator : public std::allocator<T>
{
    public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    template <class U> ICEAllocator (const ICEAllocator<U>&) noexcept;

    inline ICEAllocator() noexcept {
    }

    inline pointer allocate(size_t size, const_pointer hint = 0) {
            void* pMem = ICEMalloc(size * sizeof(T));
            return reinterpret_cast<T *>(pMem);
    }

    inline void deallocate(void* p, size_t size) {
        ICEFree(p);
    }
};


template <class T, class U>
constexpr bool operator== (const ICEAllocator<T>&, const ICEAllocator<U>&) noexcept;

template <class T, class U>
constexpr bool operator!= (const ICEAllocator<T>&, const ICEAllocator<U>&) noexcept;


