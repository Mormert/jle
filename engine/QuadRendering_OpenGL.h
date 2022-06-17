// Copyright (c) 2022. Johan Lind

#pragma once

#include "iQuadRenderingInternal.h"

#include "Shader_OpenGL.h"

#include <vector>

namespace jle {
    class QuadRendering_OpenGL : public iQuadRenderingInternal {
    public:

        QuadRendering_OpenGL();

        ~QuadRendering_OpenGL() override;

        // Inherited via iQuadRenderingInternal
        void SendTexturedQuad(TexturedQuad &texturedQuad, RenderingMethod renderingMethod) override;

        void SendColoredQuad(ColoredQuad &coloredQuad, RenderingMethod renderingMethod) override;

        void QueueRender(iFramebuffer &framebufferOut, const jleCamera &camera) override;

        void Render(iFramebuffer &framebufferOut, const jleCamera &camera, const std::vector<TexturedQuad>& texturedQuads, bool clearDepthColor) override;

        void ClearBuffersForNextFrame() override;

    private:
        jle::gfx::Shader_OpenGL quadShader;
        unsigned int quadVBO, quadVAO;

        // Instanced version
        jle::gfx::Shader_OpenGL quadShaderInstanced;
        unsigned int quadVBO_Instanced, quadVAO_Instanced, instanceVBO, elementbuffer;

        std::vector<TexturedQuad> mQueuedTexturedQuads;
    };
}



