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

#pragma once

#include "jleCommon.h"

#include "jlePathDefines.h"
#include <chrono>
#include <string_view>
#include <vector>

#include <Tracy.hpp>

class jleProfiler
{
public:
    static void NewFrame();

    struct jleProfilerData {
        std::string_view _name;

        std::chrono::duration<double, std::micro> _executionTime;

        std::vector<int> _children;
    };

    struct jleProfilerRAII {
        explicit jleProfilerRAII(std::string_view name);

        ~jleProfilerRAII();

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> _startTime;
        std::string_view _name;
        int _index;
        int _parentIndex;
    };

    static std::vector<jleProfilerData> &profilerDataLastFrame();

private:
    static inline int sCurrentProfilerData = -1;
    static inline std::vector<jleProfilerData> sProfilerData;
    static inline std::vector<jleProfilerData> sProfilerDataLastFrame;
};

#define JLE_SCOPE_PROFILE_CONCAT2(x, y) x##y
#define JLE_SCOPE_PROFILE_CONCAT(x, y) JLE_SCOPE_PROFILE_CONCAT2(x, y)
#define JLE_SCOPE_PROFILE_STRINGIZE(x) #x

#if JLE_BUILD_EDITOR
#define JLE_SCOPE_PROFILE_CPU(profile_name)                                                                            \
    ZoneScoped;                                                                                                        \
    using namespace std::literals::string_view_literals;                                                               \
    jleProfiler::jleProfilerRAII THIS_SCOPE_IS_PROFILED{                                                               \
        JLE_SCOPE_PROFILE_CONCAT(JLE_SCOPE_PROFILE_STRINGIZE(profile_name), sv)};

#define JLE_SCOPE_PROFILE_GPU(profile_name) ZoneScoped

#else
#define JLE_SCOPE_PROFILE_CPU(profile_name)
#define JLE_SCOPE_PROFILE_GPU(profile_name)
#endif
