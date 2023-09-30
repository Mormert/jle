// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleSTL/jleStringView.h"
#include <plog/Log.h>

#include <chrono>

#define JLE_SCOPED_PROFILE_LOG(message) \
    auto scopedLog = jleScopeProfileLog(message);

class jleScopeProfileLog
{
public:
    explicit jleScopeProfileLog(const jleStringView &message)
    {
        _message = message;
        _start = std::chrono::high_resolution_clock::now();
    }

    ~jleScopeProfileLog()
    {
        const auto elapsed = std::chrono::high_resolution_clock::now() - _start;
        long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        LOGI << _message.data() << " : " << ms << " (milliseconds)";
    }

private:
    jleStringView _message;
    std::chrono::time_point<std::chrono::steady_clock> _start;
};
