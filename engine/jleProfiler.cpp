// Copyright (c) 2022. Johan Lind

#include "jleProfiler.h"

jleProfiler::jleProfilerRAII::jleProfilerRAII(const std::string_view name)
        : mStartTime{std::chrono::high_resolution_clock::now()}, mName{name} {
    sProfilerData.push_back(jleProfilerData{mName});
    mIndex = sProfilerData.size() - 1;
    mParentIndex = sCurrentProfilerData;
    if(mParentIndex >= 0)
    {
        sProfilerData[mParentIndex].mChildren.push_back(mIndex);
    }
    sCurrentProfilerData = mIndex;
}

jleProfiler::jleProfilerRAII::~jleProfilerRAII() {
    const auto execTime = std::chrono::high_resolution_clock::now() - mStartTime;
    sProfilerData[mIndex].mExecutionTime = execTime;

    sCurrentProfilerData = mParentIndex;
}

void jleProfiler::NewFrame() {
    sCurrentProfilerData = -1;
    sProfilerDataLastFrame = sProfilerData;
    sProfilerData.clear();
}

std::vector<jleProfiler::jleProfilerData> &jleProfiler::GetProfilerDataLastFrame() {
    return sProfilerDataLastFrame;
}
