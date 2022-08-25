// Copyright (c) 2022. Johan Lind


#ifndef JLEPROFILER_H
#define JLEPROFILER_H

#include <string_view>
#include <chrono>
#include "jlePathDefines.h"

#include <json.hpp>

class jleProfiler {
public:

    static void NewFrame();

    struct jleProfilerData {
        std::string_view mName;

        std::chrono::duration<double, std::micro> mExecutionTime;

        std::vector<int> mChildren;
    };

    struct jleProfilerRAII {
        explicit jleProfilerRAII(std::string_view name);

        ~jleProfilerRAII();

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> mStartTime;
        std::string_view mName;
        int mIndex;
        int mParentIndex;
    };

    static std::vector<jleProfilerData> &GetProfilerDataLastFrame();

private:
    static inline int sCurrentProfilerData = -1;
    static inline std::vector<jleProfilerData> sProfilerData;
    static inline std::vector<jleProfilerData> sProfilerDataLastFrame;
};

#define JLE_SCOPE_PROFILE_CONCAT2(x, y) x ## y
#define JLE_SCOPE_PROFILE_CONCAT(x, y) JLE_SCOPE_PROFILE_CONCAT2(x, y)
#define JLE_SCOPE_PROFILE_STRINGIZE(x) #x

#ifdef BUILD_EDITOR
#define JLE_SCOPE_PROFILE(profile_name) \
using namespace std::literals::string_view_literals; \
jleProfiler::jleProfilerRAII THIS_SCOPE_IS_PROFILED{JLE_SCOPE_PROFILE_CONCAT(JLE_SCOPE_PROFILE_STRINGIZE(profile_name), sv)};
#else
#define JLE_SCOPE_PROFILE(profile_name)
#endif



#endif //JLEPROFILER_H
