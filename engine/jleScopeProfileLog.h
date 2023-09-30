// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleSTL/jleStringView.h"
#include <plog/Log.h>

#include <chrono>

#define JLE_SCOPED_PROFILE_LOG(message) auto scopedLog = jleScopeProfileLog(message);

class jleScopeProfileLog
{
public:
    explicit jleScopeProfileLog(const jleStringView &message)
    {
        _message = message;
        _start = std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::high_resolution_clock::now().time_since_epoch())
                     .count();
    }

    ~jleScopeProfileLog()
    {
        uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::high_resolution_clock::now().time_since_epoch())
                           .count();
        LOGI << _message.data() << " : " << now - _start << " (milliseconds)";
    }

private:
    jleStringView _message;
    uint64_t _start;
};
