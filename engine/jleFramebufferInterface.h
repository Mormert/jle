// Copyright (c) 2022. Johan Lind

#pragma once

class jleFramebufferInterface {
public:
    virtual ~jleFramebufferInterface() = default;

    virtual void CreateFramebuffer(unsigned int width, unsigned int height) = 0;

    virtual void CreateShadowFramebuffer(unsigned int width,
                                         unsigned int height) = 0;

    virtual void ResizeFramebuffer(unsigned int width, unsigned int height) = 0;

    virtual void BindToFramebuffer() = 0;

    virtual void BindToDefaultFramebuffer() = 0;

    virtual unsigned int GetWidth() = 0;

    virtual unsigned int GetHeight() = 0;

    virtual unsigned int GetTexture() = 0;
};