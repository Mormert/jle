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

#include "jleSystemUsageTracker.h"

#include <sstream>
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#elif __linux__
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <fstream>
#include <thread>
#elif __APPLE__
#include <mach/mach.h>
#endif

#include "modules/graphics/3rdparty/glad/glad.h"

namespace
{

#ifdef _WIN32
uint64_t FileTimeToMicroseconds(const FILETIME& ft) {
    return ((uint64_t(ft.dwHighDateTime) << 32) | ft.dwLowDateTime) / 10;
}
#endif

}

jleSystemUsageInfo
jleSystemUsageTracker::getSystemUsageInfo(bool queryOpenGLMemUsage)
{
    jleSystemUsageInfo usageInfo = {};

#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX memInfo;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&memInfo, sizeof(memInfo))) {
        usageInfo.memoryUsageKB = memInfo.WorkingSetSize / 1024;
    } else {
        usageInfo.error = "Unable to fetch memory usage on Windows.";
        return usageInfo;
    }

    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        uint64_t kernelTimeMicro = FileTimeToMicroseconds(kernelTime);
        uint64_t userTimeMicro = FileTimeToMicroseconds(userTime);
        usageInfo.cpuUsageMicroseconds = kernelTimeMicro + userTimeMicro;
        usageInfo.cpuUtilizationPercentage = static_cast<double>(usageInfo.cpuUsageMicroseconds) / (1000000.0 * std::thread::hardware_concurrency()) * 100.0;
    } else {
        usageInfo.error = "Unable to fetch CPU usage on Windows.";
        return usageInfo;
    }

    usageInfo.threadCount = GetCurrentThreadId();

#elif __linux__
    struct sysinfo memInfo;
    if (sysinfo(&memInfo) == 0) {
        usageInfo.memoryUsageKB = (memInfo.totalram - memInfo.freeram) * memInfo.mem_unit / 1024;
    } else {
        usageInfo.error = "Unable to fetch memory usage on Linux.";
        return usageInfo;
    }

    std::ifstream procStat("/proc/stat");
    if (procStat) {
        std::string line;
        while (std::getline(procStat, line)) {
            if (line.find("cpu ") == 0) {
                std::istringstream iss(line);
                std::string cpu;
                uint64_t user, nice, system, idle;
                iss >> cpu >> user >> nice >> system >> idle;
                uint64_t totalTime = user + nice + system;
                usageInfo.cpuUsageMicroseconds = totalTime * 1000;
                usageInfo.cpuUtilizationPercentage = static_cast<double>(totalTime) / (totalTime + idle) * 100.0;
                break;
            }
        }
    } else {
        usageInfo.error = "Unable to fetch CPU usage on Linux.";
        return usageInfo;
    }

    usageInfo.threadCount = std::thread::hardware_concurrency();

#elif __APPLE__
    mach_task_basic_info info;
    mach_msg_type_number_t size = MACH_TASK_BASIC_INFO_COUNT;
    kern_return_t kerr = task_info(mach_task_self(),
                                   MACH_TASK_BASIC_INFO,
                                   reinterpret_cast<task_info_t>(&info),
                                   &size);
    if (kerr == KERN_SUCCESS) {
        usageInfo.memoryUsageKB = info.resident_size / 1024;
    } else {
        usageInfo.error = "Memory info error: " + std::string(mach_error_string(kerr));
        return usageInfo;
    }

    thread_act_array_t threadList;
    mach_msg_type_number_t threadCount;
    thread_info_data_t threadInfo;
    mach_msg_type_number_t threadInfoCount;
    thread_basic_info_t basicInfo;
    uint64_t totalCPUTime = 0;
    uint64_t idleTime = 0;

    kerr = task_threads(mach_task_self(), &threadList, &threadCount);
    if (kerr != KERN_SUCCESS) {
        usageInfo.error = "Thread list error: " + std::string(mach_error_string(kerr));
        return usageInfo;
    }

    usageInfo.threadCount = static_cast<int>(threadCount);

    for (mach_msg_type_number_t i = 0; i < threadCount; i++) {
        threadInfoCount = THREAD_INFO_MAX;
        kerr = thread_info(threadList[i], THREAD_BASIC_INFO, reinterpret_cast<thread_info_t>(threadInfo), &threadInfoCount);
        if (kerr == KERN_SUCCESS) {
            basicInfo = reinterpret_cast<thread_basic_info_t>(threadInfo);
            if (!(basicInfo->flags & TH_FLAGS_IDLE)) {
                totalCPUTime += basicInfo->user_time.seconds * 1000000 + basicInfo->user_time.microseconds;
                totalCPUTime += basicInfo->system_time.seconds * 1000000 + basicInfo->system_time.microseconds;
            } else {
                idleTime += basicInfo->user_time.seconds * 1000000 + basicInfo->user_time.microseconds;
            }
        }
    }

    vm_deallocate(mach_task_self(), reinterpret_cast<vm_offset_t>(threadList), threadCount * sizeof(thread_t));

    usageInfo.cpuUsageMicroseconds = totalCPUTime;
#endif

    if(queryOpenGLMemUsage) {
        GLint totalMemoryKB = 0;
        GLint currentMemoryKB = 0;
#ifdef GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX
        glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalMemoryKB);
        glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &currentMemoryKB);
        usageInfo.openglMemoryUsageKB = totalMemoryKB - currentMemoryKB;
#else
        usageInfo.openglMemoryUsageKB = 0; // Not available
#endif
    }

    return usageInfo;
}
