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
#include "jleBuildConfig.h"

#if !defined(NDEBUG) && JLE_BUILD_MEMTRACK

#include <iostream>
#include <cstdlib>
#include <atomic>
#include <plog/Log.h>

#if defined(_WIN32)
#include <windows.h>
#include <detours/Detours/src/detours.h>
#else
#include <dlfcn.h>
#endif

namespace
{
    constexpr size_t MAX_ALLOCATIONS = 10000000;

    struct DebugAllocation {
        void *ptr;
        size_t size;
    };

    DebugAllocation g_allocationMap[MAX_ALLOCATIONS];
    size_t g_allocationCount = 0;
    size_t g_totalAllocatedBytes = 0;

    std::atomic_flag g_mallocLock = ATOMIC_FLAG_INIT;

    inline void lockMalloc()
    {
        while (g_mallocLock.test_and_set(std::memory_order_acquire)) {
            // busy wait
        }
    }

    inline void unlockMalloc()
    {
        g_mallocLock.clear(std::memory_order_release);
    }
}

size_t jleMalloc::getBytesAllocated() {
    lockMalloc();
    size_t totalBytes = g_totalAllocatedBytes;
    unlockMalloc();
    return totalBytes;
}

#if defined(_WIN32)

static LPVOID (WINAPI *original_HeapAlloc)(HANDLE, DWORD, SIZE_T) = HeapAlloc;
static BOOL   (WINAPI *original_HeapFree)(HANDLE, DWORD, LPVOID) = HeapFree;
static LPVOID (WINAPI *original_VirtualAlloc)(LPVOID, SIZE_T, DWORD, DWORD) = VirtualAlloc;
static BOOL (WINAPI *original_VirtualFree)(LPVOID, SIZE_T, DWORD) = VirtualFree;
static PVOID  (WINAPI *original_RtlAllocateHeap)(PVOID, ULONG, SIZE_T) = nullptr;
static BOOLEAN (WINAPI *original_RtlFreeHeap)(PVOID, ULONG, PVOID) = nullptr;

LPVOID WINAPI jleHooked_HeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes) {
    LPVOID ptr = original_HeapAlloc(hHeap, dwFlags, dwBytes);

    lockMalloc();
    if (g_allocationCount < MAX_ALLOCATIONS) {
        g_allocationMap[g_allocationCount++] = {ptr, dwBytes};
        g_totalAllocatedBytes += dwBytes;
    } else {
        char buffer[128];
        wsprintfA(buffer, "[jleHooked_HeapAlloc] Allocation tracking limit reached\n");
        OutputDebugStringA(buffer);
        std::abort();
    }
    unlockMalloc();

    return ptr;
}

BOOL WINAPI jleHooked_HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem) {
    lockMalloc();
    for (size_t i = 0; i < g_allocationCount; ++i) {
        if (g_allocationMap[i].ptr == lpMem) {
            g_totalAllocatedBytes -= g_allocationMap[i].size;
            g_allocationMap[i] = g_allocationMap[--g_allocationCount];
            break;
        }
    }
    unlockMalloc();

    return original_HeapFree(hHeap, dwFlags, lpMem);
}

LPVOID WINAPI jleHooked_VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect) {
    LPVOID ptr = original_VirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect);

    lockMalloc();
    if (g_allocationCount < MAX_ALLOCATIONS) {
        g_allocationMap[g_allocationCount++] = {ptr, dwSize};
        g_totalAllocatedBytes += dwSize;
    } else {
        char buffer[128];
        wsprintfA(buffer, "[jleHooked_VirtualAlloc] Allocation tracking limit reached\n");
        OutputDebugStringA(buffer);
        std::abort();
    }
    unlockMalloc();

    return ptr;
}

PVOID WINAPI jleHooked_RtlAllocateHeap(PVOID HeapHandle, ULONG Flags, SIZE_T Size) {
    PVOID ptr = original_RtlAllocateHeap(HeapHandle, Flags, Size);

    lockMalloc();
    if (g_allocationCount < MAX_ALLOCATIONS) {
        g_allocationMap[g_allocationCount++] = {ptr, Size};
        g_totalAllocatedBytes += Size;
    } else {
        char buffer[128];
        wsprintfA(buffer, "[jleHooked_RtlAllocateHeap] Allocation tracking limit reached\n");
        OutputDebugStringA(buffer);
        std::abort();
    }
    unlockMalloc();

    return ptr;
}

BOOLEAN WINAPI jleHooked_RtlFreeHeap(PVOID HeapHandle, ULONG Flags, PVOID BaseAddress) {
    lockMalloc();
    for (size_t i = 0; i < g_allocationCount; ++i) {
        if (g_allocationMap[i].ptr == BaseAddress) {
            g_totalAllocatedBytes -= g_allocationMap[i].size;
            g_allocationMap[i] = g_allocationMap[--g_allocationCount];
            break;
        }
    }
    unlockMalloc();

    return original_RtlFreeHeap(HeapHandle, Flags, BaseAddress);
}

BOOL WINAPI jleHooked_VirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType) {
    lockMalloc();
    for (size_t i = 0; i < g_allocationCount; ++i) {
        if (g_allocationMap[i].ptr == lpAddress) {
            g_totalAllocatedBytes -= g_allocationMap[i].size;
            g_allocationMap[i] = g_allocationMap[--g_allocationCount];
            break;
        }
    }
    unlockMalloc();

    return original_VirtualFree(lpAddress, dwSize, dwFreeType);
}

#else

void* (*real_malloc)(size_t) = nullptr;
void (*real_free)(void*) = nullptr;

void* jle_malloc(size_t size) {
    void* ptr = real_malloc(size);
    if (!ptr) {
        return nullptr;
    }


    lockMalloc();
    if (g_allocationCount < MAX_ALLOCATIONS) {
        g_allocationMap[g_allocationCount++] = {ptr, size};
        g_totalAllocatedBytes += size;
    } else {
        std::cerr << "Error: Allocation tracking limit reached!" << std::endl;
        std::abort();
    }
    unlockMalloc();

    return ptr;
}

void jle_free(void* ptr) {
    if (ptr) {
        lockMalloc();
        for (size_t i = 0; i < g_allocationCount; ++i) {
            if (g_allocationMap[i].ptr == ptr) {
                g_totalAllocatedBytes -= g_allocationMap[i].size;
                g_allocationMap[i] = g_allocationMap[--g_allocationCount];
                break;
            }
        }
        unlockMalloc();
    }

    real_free(ptr);
}

static void* (*jle_malloc_ptr)(size_t) = std::malloc;
static void (*jle_free_ptr)(void*) = std::free;

extern "C"
{
    void* malloc(size_t size)
    {
        return jle_malloc_ptr(size);
    }

    void free(void* ptr)
    {
        jle_free_ptr(ptr);
    }
}

#endif

unsigned long jleMalloc::InstallMemTrackingHooks() {
#if defined(_WIN32)
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    original_HeapAlloc = HeapAlloc;
    original_HeapFree = HeapFree;
    original_VirtualAlloc = VirtualAlloc;
    original_VirtualFree = VirtualFree;

    original_RtlAllocateHeap = (PVOID (WINAPI*)(PVOID, ULONG, SIZE_T)) GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlAllocateHeap");
    if (!original_RtlAllocateHeap) {
        LOGE << "Failed to find RtlAllocateHeap";
        return ERROR_PROC_NOT_FOUND;
    }

    original_RtlFreeHeap = (BOOLEAN (WINAPI*)(PVOID, ULONG, PVOID)) GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlFreeHeap");
    if (!original_RtlFreeHeap) {
        LOGE << "Failed to find RtlFreeHeap";
        return ERROR_PROC_NOT_FOUND;
    }

    DetourAttach((PVOID*)&original_HeapAlloc, jleHooked_HeapAlloc);
    DetourAttach((PVOID*)&original_HeapFree, jleHooked_HeapFree);
    DetourAttach((PVOID*)&original_VirtualAlloc, jleHooked_VirtualAlloc);
    DetourAttach((PVOID*)&original_VirtualFree, jleHooked_VirtualFree);
    DetourAttach((PVOID*)&original_RtlAllocateHeap, jleHooked_RtlAllocateHeap);
    DetourAttach((PVOID*)&original_RtlFreeHeap, jleHooked_RtlFreeHeap);

    return DetourTransactionCommit();
#else
    if (!real_malloc) {
        real_malloc = reinterpret_cast<void* (*)(size_t)>(dlsym(RTLD_NEXT, "malloc"));
        if (!real_malloc) {
            std::cerr << "Error: Unable to find the real malloc function!" << std::endl;
            std::abort();
        }
    }

    if (!real_free) {
        real_free = reinterpret_cast<void (*)(void*)>(dlsym(RTLD_NEXT, "free"));
        if (!real_free) {
            std::cerr << "Error: Unable to find the real free function!" << std::endl;
            std::abort();
        }
    }

    jle_malloc_ptr = jle_malloc;
    jle_free_ptr = jle_free;

    return 1;
#endif
}

#else // #if !defined(NDEBUG) && JLE_BUILD_MEMTRACK

size_t jleMalloc::getBytesAllocated() {
    return 0;
}

unsigned long jleMalloc::InstallMemTrackingHooks() {
    return 0;
}

#endif