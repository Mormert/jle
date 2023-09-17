// Copyright (c) 2023. Johan Lind

#ifndef JLE_NEW
#define JLE_NEW

#include <new>
#include <cstddef>

// Required by EASTL
void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line);
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line);

void* operator new(std::size_t n);
void operator delete(void* ptr) noexcept;

#endif // JLE_NEW
