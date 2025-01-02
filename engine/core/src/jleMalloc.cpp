/*********************************************************************************************
*                                                                                           *
*               ,     .     ,                      .   ,--.                                 *
*               |     |     |                      |   |            o                       *
*               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
*               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
*              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
*                                                                                           *
*     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
*          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
*     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
*                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
*                                                                                           *
*********************************************************************************************/

#include "jleMalloc.h"

#ifndef NDEBUG

#include <iostream>
#include <cstdlib>
#include <atomic>

#if defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace {
    constexpr size_t MAX_ALLOCATIONS = 10000000;

    struct Allocation {
        void* ptr;
        size_t size;
    };

    Allocation allocationMap[MAX_ALLOCATIONS];
    size_t allocationCount = 0;
    size_t totalAllocatedBytes = 0;

    std::atomic_flag mallocLock = ATOMIC_FLAG_INIT;

    // Function pointers to the original malloc/free
    // Note: On Windows, the C runtime might be in different DLLs depending on compiler flags.
    void* (*real_malloc)(size_t) = nullptr;
    void (*real_free)(void*) = nullptr;

    inline void lockMalloc() {
        while (mallocLock.test_and_set(std::memory_order_acquire)) {
            // busy wait
        }
    }

    inline void unlockMalloc() {
        mallocLock.clear(std::memory_order_release);
    }

    #if defined(_WIN32)

    // Retrieve the real 'malloc' and 'free' from the CRT on Windows.
    void initRealAllocators() {
        if (!real_malloc || !real_free) {
            // Typically, the C runtime can be found in 'ucrtbase.dll', 'msvcrt.dll', or
            // something similar, depending on the platform and Visual Studio version.
            // This is just an example name—adjust it if needed.
            HMODULE crtModule = GetModuleHandleA("ucrtbase.dll");
            if (!crtModule) {
                // Try MSVCRT
                crtModule = GetModuleHandleA("msvcrt.dll");
            }
            if (!crtModule) {
                std::cerr << "Error: Unable to find CRT module!" << std::endl;
                std::abort();
            }
            real_malloc = reinterpret_cast<void* (*)(size_t)>(
                GetProcAddress(crtModule, "malloc"));
            real_free = reinterpret_cast<void (*)(void*)>(
                GetProcAddress(crtModule, "free"));

            if (!real_malloc || !real_free) {
                std::cerr << "Error: Unable to locate real malloc/free in CRT!" << std::endl;
                std::abort();
            }
        }
    }

    #else

    void initRealAllocators() {
        if (!real_malloc) {
            real_malloc = reinterpret_cast<void* (*)(size_t)>(
                dlsym(RTLD_NEXT, "malloc"));
            if (!real_malloc) {
                std::cerr << "Error: Unable to find the real malloc function!" << std::endl;
                std::abort();
            }
        }

        if (!real_free) {
            real_free = reinterpret_cast<void (*)(void*)>(
                dlsym(RTLD_NEXT, "free"));
            if (!real_free) {
                std::cerr << "Error: Unable to find the real free function!" << std::endl;
                std::abort();
            }
        }
    }
    #endif

    void* jle_malloc(size_t size) {
        initRealAllocators();

        void* ptr = real_malloc(size);
        if (!ptr) {
            return nullptr;
        }


        lockMalloc();
        if (allocationCount < MAX_ALLOCATIONS) {
            allocationMap[allocationCount++] = {ptr, size};
            totalAllocatedBytes += size;
        } else {
            std::cerr << "Error: Allocation tracking limit reached!" << std::endl;
            std::abort();
        }
        unlockMalloc();

        return ptr;
    }

    void jle_free(void* ptr) {
        initRealAllocators();

        if (ptr) {
            lockMalloc();
            for (size_t i = 0; i < allocationCount; ++i) {
                if (allocationMap[i].ptr == ptr) {
                    totalAllocatedBytes -= allocationMap[i].size;
                    allocationMap[i] = allocationMap[--allocationCount];
                    break;
                }
            }
            unlockMalloc();
        }

        real_free(ptr);
    }
}

size_t jleMalloc::getBytesAllocated() {
    lockMalloc();
    size_t totalBytes = totalAllocatedBytes;
    unlockMalloc();
    return totalBytes;
}

extern "C" {
void* malloc(size_t size) {
    return jle_malloc(size);
}

void free(void* ptr) {
    jle_free(ptr);
}
}

#endif // !NDEBUG