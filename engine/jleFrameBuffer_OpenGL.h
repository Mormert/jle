// Copyright (c) 2022. Johan Lind

#pragma once

class Framebuffer_OpenGL {
public:
    Framebuffer_OpenGL(unsigned int width,
                       unsigned int height,
                       bool shadowBuffer = false);

    ~Framebuffer_OpenGL();

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
