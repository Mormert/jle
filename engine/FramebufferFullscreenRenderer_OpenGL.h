// Copyright (c) 2022. Johan Lind

#pragma once

#include "iFramebufferFullscreenRenderer.h"

#include "Shader_OpenGL.h"

namespace jle {
    class FramebufferFullscreenRenderer_OpenGL : iFramebufferFullscreenRenderer {
    public:
        ~FramebufferFullscreenRenderer_OpenGL() override;

        FramebufferFullscreenRenderer_OpenGL();

        // Inherited via iFramebufferFullscreenRenderer
        void RenderFramebufferFullscreen(iFramebuffer &framebuffer, unsigned int screenWidth,
                                         unsigned int screenHeight) override;

    private:
        jle::gfx::Shader_OpenGL quadScreenShader;
        unsigned int quadVAO, quadVBO;
    };
}

