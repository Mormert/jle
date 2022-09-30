// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleQuadRenderingInterface.h"
#include "jleTextRenderingInterface.h"

#include <memory>

class jleRenderingAPIInterface {
public:
    virtual ~jleRenderingAPIInterface() = default;
    ;

    std::unique_ptr<jleQuadRenderingInterface> quads;
    std::unique_ptr<jleTextRenderingInterface> texts;
};
