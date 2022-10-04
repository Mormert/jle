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

class jleFramebuffer;

class jleQuadRendering {
public:
    static inline glm::vec3 lightPos{};
    static inline float depthRange{10000.f};

    jleQuadRendering();

    ~jleQuadRendering();

    // Inherited via iQuadRenderingInternal
    void sendTexturedQuad(texturedQuad &texturedQuad);

    void sendTexturedHeightQuad(texturedHeightQuad &texturedHeightQuad);

    void sendSimpleTexturedHeightQuad(texturedHeightQuad &texturedHeightQuad);

    void queuerender(jleFramebuffer &framebufferOut, jleCamera &camera);

    void render(
        jleFramebuffer &framebufferOut,
        jleCamera &camera,
        const std::vector<texturedQuad> &texturedQuads,
        const std::vector<texturedHeightQuad> &texturedHeightQuads,
        const std::vector<texturedHeightQuad> &texturedSimpleHeightQuads,
        bool clearDepthColor);

    void clearBuffersForNextFrame();

private:
    jleShader quadShader;
    unsigned int quadVBO, quadVAO;

    void setupShaders();

    void processTexturedQuads(const std::vector<texturedQuad> &texturedQuads,
                              glm::mat4 &view);

    void processTexturedHeightQuads(
        const std::vector<texturedHeightQuad> &texturedHeightQuads,
        glm::mat4 &view,
        glm::vec3 viewPos);

    void processSimpleTexturedHeightQuads(
        const std::vector<texturedHeightQuad> &texturedHeightQuads,
        glm::mat4 &view,
        glm::vec3 viewPos);

    float xyAngle = 0.f;
    float zAngle = 90.f - 35.24f;
    float cosXY = cos(xyAngle * glm::pi<float>() / 180.0);
    const float sinXY = sin(xyAngle * glm::pi<float>() / 180.0);
    const float cosZ = cos(zAngle * glm::pi<float>() / 180.0);
    const float sinZ = sin(zAngle * glm::pi<float>() / 180.0);

    // --- For shadow purposes only (WIP)
    void renderCube(glm::mat4 &model, jleShader &shader);

    jleShader shadowMappingShader;
    std::unique_ptr<jleFramebuffer> shadowMapBuffer;

    void renderShadowCubes(glm::mat4 &view);
    // ---

    // Instanced version
    jleShader quadShaderInstanced;
    jleShader quadHeightmapShaderInstanced;
    jleShader quadHeightmapShaderInstancedSimple;
    unsigned int quadVBO_Instanced, quadVAO_Instanced, instanceVBO,
        elementbuffer;

    std::vector<texturedQuad> _queuedTexturedQuads;
    std::vector<texturedHeightQuad> _queuedTexturedHeightQuads;
    std::vector<texturedHeightQuad> _queuedSimpleTexturedHeightQuads;
};
