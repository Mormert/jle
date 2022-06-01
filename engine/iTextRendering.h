// Copyright (c) 2022. Johan Lind

#pragma once

#include <string>

class iTextRendering {
public:
    virtual ~iTextRendering() = default;

    virtual void SendSimpleText(const std::string &text, float x, float y,
                                float scale = 1.f, float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f) = 0;
};