// Copyright (c) 2023. Johan Lind

#include "jleDynamicLogAppender.h"
#include <memory>

plog::DynamicAppender &
dynamicAppender()
{
    static std::unique_ptr<plog::DynamicAppender> _dynamicAppender{};

    if(!_dynamicAppender){
        _dynamicAppender = std::make_unique<plog::DynamicAppender>();
    }

    return *_dynamicAppender;
}
