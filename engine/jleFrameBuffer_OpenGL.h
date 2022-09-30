// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFramebufferInterface.h"

class Framebuffer_OpenGL : public jleFramebufferInterface {
public:
    Framebuffer_OpenGL(unsigned int width,
                       unsigned int height,
                       bool shadowBuffer = false);

    ~Framebuffer_OpenGL() override;

    void CreateFramebuffer(unsigned int width, unsigned int height) override;

    void CreateShadowFramebuffer(unsigned int width,
                                 unsigned int height) override;

    void ResizeFramebuffer(unsigned int width, unsigned int height) override;

    void BindToFramebuffer() override;

    void BindToDefaultFramebuffer() override;

    unsigned int GetWidth() override;

    unsigned int GetHeight() override;

    unsigned int GetTexture() override;

private:
    unsigned int width{}, height{};
    unsigned int framebuffer{};
    unsigned int texColorBuffer{};
    unsigned int rbo{};
};
