// Copyright (c) 2022. Johan Lind

#pragma once

#include <glm/glm.hpp>

class jleFramebuffer
{
public:
    jleFramebuffer(unsigned int width, unsigned int height, bool shadowBuffer = false);

    ~jleFramebuffer();

    void createRenderFramebuffer(unsigned int width, unsigned int height);

    void createShadowMappingFramebuffer(unsigned int width, unsigned int height);

    void resize(unsigned int width, unsigned int height);

    // Specifies which axis to be fixed, and the other scales depending on window aspect ratio
    enum class FIXED_AXIS { width, height };

    static glm::ivec2 fixedAxisDimensions(jleFramebuffer::FIXED_AXIS fixedAxis,
                                          float aspect,
                                          unsigned int fixedAxisPixels);
    void bind();

    void bindDefault();

    unsigned int width();

    unsigned int height();

    unsigned int texture();

private:
    unsigned int _width{}, _height{};

    // The framebuffer ID
    unsigned int _framebuffer{};

    // The framebuffer's texture ID
    unsigned int _texColorBuffer{};

    // The renderbuffer ID. Used for depth buffering when doing off-screen rendering.
    // Can be unset.
    unsigned int _rbo{};

    bool _isShadowFramebuffer{false};
};
