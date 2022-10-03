// Copyright (c) 2022. Johan Lind

#pragma once

class jleFramebuffer {
public:
    jleFramebuffer(unsigned int width,
                       unsigned int height,
                       bool shadowBuffer = false);

    ~jleFramebuffer();

    void CreateFramebuffer(unsigned int width, unsigned int height);

    void CreateShadowFramebuffer(unsigned int width, unsigned int height);

    void ResizeFramebuffer(unsigned int width, unsigned int height);

    void BindToFramebuffer();

    void BindToDefaultFramebuffer();

    unsigned int GetWidth();

    unsigned int GetHeight();

    unsigned int GetTexture();

private:
    unsigned int width{}, height{};
    unsigned int framebuffer{};
    unsigned int texColorBuffer{};
    unsigned int rbo{};
};