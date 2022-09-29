// Copyright (c) 2022. Johan Lind

#pragma once

#include "iQuadRenderingInternal.h"

#include "Shader_OpenGL.h"
#include "glm/ext/scalar_constants.hpp"

#include <glm/glm.hpp>
#include <vector>

namespace jle {
    class QuadRendering_OpenGL : public iQuadRenderingInternal {
    public:

        static inline glm::vec3 lightPos{};
        static inline float depthRange{10000.f};

        QuadRendering_OpenGL();

        ~QuadRendering_OpenGL() override;

        // Inherited via iQuadRenderingInternal
        void SendTexturedQuad(TexturedQuad &texturedQuad, RenderingMethod renderingMethod) override;

        void SendTexturedHeightQuad(TexturedHeightQuad &texturedHeightQuad, RenderingMethod renderingMethod) override;

        void SendColoredQuad(ColoredQuad &coloredQuad, RenderingMethod renderingMethod) override;

        void QueueRender(iFramebuffer &framebufferOut, jleCamera &camera) override;

        void Render(iFramebuffer &framebufferOut, jleCamera &camera, const std::vector<TexturedQuad>& texturedQuads, const std::vector<TexturedHeightQuad>& texturedHeightQuads, bool clearDepthColor) override;

        void ClearBuffersForNextFrame() override;

    private:
        jle::gfx::Shader_OpenGL quadShader;
        unsigned int quadVBO, quadVAO;

        void SetupShaders();

        void ProcessTexturedQuads(const std::vector<TexturedQuad>& texturedQuads, glm::mat4& view);

        void ProcessTexturedHeightQuads(const std::vector<TexturedHeightQuad>& texturedHeightQuads, glm::mat4& view, glm::vec3 viewPos);

        float xyAngle = 0.f;
        float zAngle = 90.f - 35.24f;
        float cosXY = cos(xyAngle * glm::pi<float>() / 180.0);
        const float sinXY = sin(xyAngle * glm::pi<float>() / 180.0);
        const float cosZ = cos(zAngle * glm::pi<float>() / 180.0);
        const float sinZ = sin(zAngle * glm::pi<float>() / 180.0);

        // --- For shadow purposes only (WIP)
        void RenderCube(glm::mat4& model, gfx::Shader_OpenGL& shader);
        jle::gfx::Shader_OpenGL shadowMappingShader;
        std::unique_ptr<iFramebuffer> shadowMapBuffer;
        void RenderShadowCubes(glm::mat4 & view);
        // ---

        // Instanced version
        jle::gfx::Shader_OpenGL quadShaderInstanced;
        jle::gfx::Shader_OpenGL quadHeightmapShaderInstanced;
        unsigned int quadVBO_Instanced, quadVAO_Instanced, instanceVBO, elementbuffer;

        std::vector<TexturedQuad> mQueuedTexturedQuads;
        std::vector<TexturedHeightQuad> mQueuedTexturedHeightQuads;
    };
}



