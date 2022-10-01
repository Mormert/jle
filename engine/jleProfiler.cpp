// Copyright (c) 2022. Johan Lind

#include "jleProfiler.h"

jleProfiler::jleProfilerRAII::jleProfilerRAII(const std::string_view name)
    : _startTime{std::chrono::high_resolution_clock::now()}, _name{name} {
    sProfilerData.push_back(jleProfilerData{_name});
    _index = sProfilerData.size() - 1;
    _parentIndex = sCurrentProfilerData;
    if (_parentIndex >= 0) {
        sProfilerData[_parentIndex]._children.push_back(_index);
    }
    sCurrentProfilerData = _index;
}

jleProfiler::jleProfilerRAII::~jleProfilerRAII() {
    const auto execTime =
        std::chrono::high_resolution_clock::now() - _startTime;
    sProfilerData[_index]._executionTime = execTime;

    sCurrentProfilerData = _parentIndex;
}

void jleProfiler::NewFrame() {
    sCurrentProfilerData = -1;
    sProfilerDataLastFrame = sProfilerData;
    sProfilerData.clear();
}

std::vector<jleProfiler::jleProfilerData>& jleProfiler::
    GetProfilerDataLastFrame() {
    return sProfilerDataLastFrame;
}
