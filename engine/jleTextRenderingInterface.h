// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFont.h"

#include <string>

class jleTextRenderingInterface {
public:
    virtual ~jleTextRenderingInterface() = default;

    virtual void SendSimpleText(const std::string& text,
                                float x,
                                float y,
                                float scale = 1.f,
                                float r = 1.f,
                                float g = 1.f,
                                float b = 1.f,
                                float a = 1.f) = 0;

    virtual void SendFontText(jleFont *font,
                              const std::string& text,
                              uint32_t fontSize,
                              float x,
                              float y,
                              float depth,
                              float r = 1.f,
                              float g = 1.f,
                              float b = 1.f,
                              float a = 1.f) = 0;
};