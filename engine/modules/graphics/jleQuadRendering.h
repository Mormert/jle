/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#pragma once

#include "core/jleCommon.h"

#include "core/jleCamera.h"
#include "core/jleResourceRef.h"
#include "glm/ext/scalar_constants.hpp"
#include "jleQuads.h"
#include "jleShader.h"
#include "jleTexture.h"
#include <glm/glm.hpp>
#include <memory>
#include <utility>
#include <vector>

class jleFramebufferInterface;

class jleQuadRendering {
public:
    static inline glm::vec3 lightPos{};
    static inline float depthRange{10000.f};

    jleQuadRendering();

    ~jleQuadRendering();

    // Inherited via iQuadRenderingInternal
    void sendTexturedQuad(texturedQuad &texturedQuad);

    void sendTexturedHeightQuad(jleTexturedHeightQuad &texturedHeightQuad);

    void sendSimpleTexturedHeightQuad(
        jleTexturedHeightQuad &texturedHeightQuad);

    void queuerender(jleFramebufferInterface &framebufferOut, const jleCamera &camera);

    void render(jleFramebufferInterface &framebufferOut,
                const jleCamera &camera,
                const std::vector<texturedQuad> &texturedQuads,
                const std::vector<jleTexturedHeightQuad> &texturedHeightQuads,
                const std::vector<jleTexturedHeightQuad> &texturedSimpleHeightQuads);

    void clearBuffersForNextFrame();

    void renderShadowCubes(const glm::mat4 &view);

private:
    jleResourceRef<jleShader> quadShader;
    unsigned int quadVBO, quadVAO;

    void setupShaders();

    void processTexturedQuads(const std::vector<texturedQuad> &texturedQuads, glm::mat4 &view);

    void processTexturedHeightQuads(
        const std::vector<jleTexturedHeightQuad> &texturedHeightQuads,
        glm::mat4 &view,
        glm::vec3 viewPos);

    void processSimpleTexturedHeightQuads(
        const std::vector<jleTexturedHeightQuad> &texturedHeightQuads,
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

    jleResourceRef<jleShader> shadowMappingShader;
    std::unique_ptr<jleFramebufferInterface> shadowMapBuffer;

    // ---

    // Instanced version
    jleResourceRef<jleShader> quadShaderInstanced;
    jleResourceRef<jleShader> quadHeightmapShaderInstanced;
    jleResourceRef<jleShader> quadHeightmapShaderInstancedSimple;
    unsigned int quadVBO_Instanced, quadVAO_Instanced, instanceVBO,
        elementbuffer;

    std::vector<texturedQuad> _queuedTexturedQuads;
    std::vector<jleTexturedHeightQuad> _queuedTexturedHeightQuads;
    std::vector<jleTexturedHeightQuad> _queuedSimpleTexturedHeightQuads;
};
