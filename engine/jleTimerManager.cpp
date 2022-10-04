// Copyright (c) 2022. Johan Lind

#include "jleTimerManager.h"
#include <GLFW/glfw3.h>

#include "jleProfiler.h"

void jleTimerManager::process() {
    JLE_SCOPE_PROFILE(jleTimerManager::process)
    const auto timeNow = glfwGetTime();

    if (!_functionsSharedData.empty()) {
        for (auto it = _functionsSharedData.cbegin();
             it != _functionsSharedData.cend() /**/;
             /**/) {
            if (timeNow >= it->first) {
                const auto data = it->second._data;
                const auto function = it->second._function;

                function(data); // execute!

                _functionsSharedData.erase(it++);
            }
            else {
                break;
            }
        }
    }

    if (!_functionsWeakData.empty()) {
        for (auto it = _functionsWeakData.cbegin();
             it != _functionsWeakData.cend() /**/;
             /**/) {
            if (timeNow >= it->first) {
                const auto data = it->second._data;
                const auto function = it->second._function;

                if (!data.expired()) {
                    function(data); // execute!
                }

                _functionsWeakData.erase(it++);
            }
            else {
                break;
            }
        }
    }
}

void jleTimerManager::executeFuncInSecondsSharedData(
    double seconds,
    void (*f)(std::shared_ptr<void>),
    const std::shared_ptr<void> &data) {
    jleTimerManagerFunctionSharedData functionAndData{f, data};

    double futureTime = (glfwGetTime() + seconds);
    _functionsSharedData.insert({futureTime, functionAndData});
}

void jleTimerManager::executeFuncInSecondsWeakData(
    double seconds,
    void (*f)(std::weak_ptr<void>),
    const std::weak_ptr<void> &data) {
    jleTimerManagerFunctionWeakData functionAndData{f, data};

    double futureTime = (glfwGetTime() + seconds);
    _functionsWeakData.insert({futureTime, functionAndData});
}

void jleTimerManager::clearTimers() {
    _functionsSharedData.clear();
    _functionsWeakData.clear();
}
