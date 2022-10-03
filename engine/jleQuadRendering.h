// Copyright (c) 2022. Johan Lind

#pragma once

#include "glm/ext/scalar_constants.hpp"
#include "jleCamera.h"
#include "jleQuads.h"
#include "jleShader.h"
#include "jleTexture.h"
#include <glm/glm.hpp>
#include <memory>
#include <utility>
#include <vector>

enum class RenderingMethod { Dynamic, Static };

class Framebuffer_OpenGL;

class jleQuadRendering {
public:
    static inline glm::vec3 lightPos{};
    static inline float depthRange{10000.f};

    jleQuadRendering();

    ~jleQuadRendering();

    // Inherited via iQuadRenderingInternal
    void SendTexturedQuad(TexturedQuad& texturedQuad,
                          RenderingMethod renderingMethod);

    void SendTexturedHeightQuad(TexturedHeightQuad& texturedHeightQuad,
                                RenderingMethod renderingMethod);

    void SendColoredQuad(ColoredQuad& coloredQuad,
                         RenderingMethod renderingMethod);

    void QueueRender(Framebuffer_OpenGL& framebufferOut, jleCamera& camera);

    void Render(Framebuffer_OpenGL& framebufferOut,
                jleCamera& camera,
                const std::vector<TexturedQuad>& texturedQuads,
                const std::vector<TexturedHeightQuad>& texturedHeightQuads,
                bool clearDepthColor);

    void ClearBuffersForNextFrame();

private:
    jleShader quadShader;
    unsigned int quadVBO, quadVAO;

    void SetupShaders();

    void ProcessTexturedQuads(const std::vector<TexturedQuad>& texturedQuads,
                              glm::mat4& view);

    void ProcessTexturedHeightQuads(
        const std::vector<TexturedHeightQuad>& texturedHeightQuads,
        glm::mat4& view,
        glm::vec3 viewPos);

    float xyAngle = 0.f;
    float zAngle = 90.f - 35.24f;
    float cosXY = cos(xyAngle * glm::pi<float>() / 180.0);
    const float sinXY = sin(xyAngle * glm::pi<float>() / 180.0);
    const float cosZ = cos(zAngle * glm::pi<float>() / 180.0);
    const float sinZ = sin(zAngle * glm::pi<float>() / 180.0);

    // --- For shadow purposes only (WIP)
    void RenderCube(glm::mat4& model, jleShader& shader);

    jleShader shadowMappingShader;
    std::unique_ptr<Framebuffer_OpenGL> shadowMapBuffer;

    void RenderShadowCubes(glm::mat4& view);
    // ---

    // Instanced version
    jleShader quadShaderInstanced;
    jleShader quadHeightmapShaderInstanced;
    unsigned int quadVBO_Instanced, quadVAO_Instanced, instanceVBO,
        elementbuffer;

    std::vector<TexturedQuad> _queuedTexturedQuads;
    std::vector<TexturedHeightQuad> _queuedTexturedHeightQuads;
};