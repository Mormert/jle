// Copyright (c) 2022. Johan Lind

#pragma once

class jleFramebuffer {
public:
    jleFramebuffer(unsigned int width,
                   unsigned int height,
                   bool shadowBuffer = false);

    ~jleFramebuffer();

    void createFramebuffer(unsigned int width, unsigned int height);

    void createShadowFramebuffer(unsigned int width, unsigned int height);

    void resize(unsigned int width, unsigned int height);

    void bind();

    void bindDefault();

    unsigned int width();

    unsigned int height();

    unsigned int texture();

private:
    unsigned int _width{}, _height{};
    unsigned int _framebuffer{};
    unsigned int _texColorBuffer{};
    unsigned int _rbo{};
};
