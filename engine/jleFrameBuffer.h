// Copyright (c) 2022. Johan Lind

#pragma once

#include <glm/glm.hpp>

class jleFramebuffer
{
public:
    enum class jleFramebufferType {
        ScreenRenderBuffer,      // A framebuffer that renders to the screen
        MultisampleRenderBuffer, // A framebuffer that supports anti-aliasing (MSAA)
        ShadowMapBuffer          // A framebuffer that light sources render to for shadow mapping
    };

    jleFramebuffer(unsigned int width, unsigned int height, jleFramebufferType type);

    ~jleFramebuffer();

    void createScreenRenderFramebuffer(unsigned int width, unsigned int height);

    void createShadowMappingFramebuffer(unsigned int width, unsigned int height);

    void createMultisampledRenderFramebuffer(unsigned int width, unsigned int height, unsigned int samples);

    static void blitFramebuffers(jleFramebuffer &source, jleFramebuffer &target);

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
    void destroyOldBuffers();

    unsigned int _width{}, _height{};

    // The framebuffer object ID
    unsigned int _framebuffer{};

    // The framebuffer's texture ID
    // Can be unset.
    unsigned int _texColorBuffer{};

    // The renderbuffer ID. Used for depth buffering when doing off-screen rendering.
    // Can be unset.
    unsigned int _rbo{};

    // Second renderbuffer object. Used for depth buffering or color buffering.
    // Can be unset.
    unsigned int _rbo2{};

    jleFramebufferType _type;

    unsigned int _msaaSamples{4};
};
