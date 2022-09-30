// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleWindowInternalAPIInterface.h"

class jleWindowLinkableInterface {
public:
    virtual ~jleWindowLinkableInterface() = default;

    virtual void LinkWindow(
        std::shared_ptr<jleWindowInternalAPIInterface> windowInternal) = 0;
};