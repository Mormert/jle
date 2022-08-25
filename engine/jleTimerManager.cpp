// Copyright (c) 2022. Johan Lind

#include "jleTimerManager.h"
#include <GLFW/glfw3.h>

#include "jleProfiler.h"

namespace jle {
    void jleTimerManager::Process() {
        JLE_SCOPE_PROFILE(jleTimerManager::Process)
        const auto timeNow = glfwGetTime();

        if (!mFunctionsSharedData.empty()) {
                for (auto it = mFunctionsSharedData.cbegin(); it != mFunctionsSharedData.cend() /**/ ; /**/ ) {
                    if (timeNow >= it->first) {
                        const auto data = it->second.mData;
                        const auto function = it->second.mFunction;

                        function(data); // execute!

                        mFunctionsSharedData.erase(it++);
                    } else {
                        break;
                    }
                }
        }

        if (!mFunctionsWeakData.empty()) {
                for (auto it = mFunctionsWeakData.cbegin(); it != mFunctionsWeakData.cend() /**/ ; /**/ ) {
                    if (timeNow >= it->first) {
                        const auto data = it->second.mData;
                        const auto function = it->second.mFunction;

                        if (!data.expired()) {
                            function(data); // execute!
                        }

                        mFunctionsWeakData.erase(it++);
                    } else {
                        break;
                    }
                }
        }


    }

    void jleTimerManager::ExecuteFuncInSecondsSharedData(double seconds, void (*f)(std::shared_ptr<void>),
                                                         const std::shared_ptr<void> &data) {
        jleTimerManagerFunctionSharedData functionAndData{f, data};

        double futureTime = (glfwGetTime() + seconds);
        mFunctionsSharedData.insert({futureTime, functionAndData});
    }

    void jleTimerManager::ExecuteFuncInSecondsWeakData(double seconds, void (*f)(std::weak_ptr<void>),
                                                       const std::weak_ptr<void> &data) {
        jleTimerManagerFunctionWeakData functionAndData{f, data};

        double futureTime = (glfwGetTime() + seconds);
        mFunctionsWeakData.insert({futureTime, functionAndData});
    }

    void jleTimerManager::ClearTimers() {
        mFunctionsSharedData.clear();
        mFunctionsWeakData.clear();
    }

}
