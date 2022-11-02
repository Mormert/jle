// Copyright (c) 2022. Johan Lind

#include "jleQuadRendering.h"
#include "jleFrameBuffer.h"
#include "jlePathDefines.h"
#include "jleProfiler.h"
#include "plog/Log.h"
#include <random>
#include <thread>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else

#include <glad/glad.h>

#endif

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <string>
#include <unordered_map>

struct QuadData {
    float x, y, depth;
    float tex_x, tex_y, tex_w, tex_h;
};

jleQuadRendering::jleQuadRendering()
    : quadShader{std::string{JLE_ENGINE_PATH_SHADERS + "/quad.vert"}.c_str(),
                 std::string{JLE_ENGINE_PATH_SHADERS + "/quad.frag"}.c_str()},
      quadShaderInstanced{
          std::string{JLE_ENGINE_PATH_SHADERS + "/quadInstanced.vert"}.c_str(),
          std::string{JLE_ENGINE_PATH_SHADERS + "/quadInstanced.frag"}.c_str()},
      quadHeightmapShaderInstanced{
          std::string{JLE_ENGINE_PATH_SHADERS + "/quadHeightmapInstanced.vert"}
              .c_str(),
          std::string{JLE_ENGINE_PATH_SHADERS + "/quadHeightmapInstanced.frag"}
              .c_str()},
      quadHeightmapShaderInstancedSimple{
          std::string{JLE_ENGINE_PATH_SHADERS +
                      "/quadHeightmapInstancedSimple.vert"}
              .c_str(),
          std::string{JLE_ENGINE_PATH_SHADERS +
                      "/quadHeightmapInstancedSimple.frag"}
              .c_str()},
      shadowMappingShader{
          std::string{JLE_ENGINE_PATH_SHADERS + "/shadowMapping.vert"}.c_str(),
          std::string{JLE_ENGINE_PATH_SHADERS + "/shadowMapping.frag"}
              .c_str()} {

    LOG_VERBOSE << "Constructing OpenGL Quad Rendering";

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    // x, y, u, v
    constexpr float vertices[] = {0.0f,
                                  0.0f,
                                  0.0f,
                                  0.0f, // bottom left corner
                                  0.0f,
                                  1.0f,
                                  0.0f,
                                  1.0f, // top left corner
                                  1.0f,
                                  1.0f,
                                  1.0f,
                                  1.0f, // top right corner
                                  1.0f,
                                  0.0f,
                                  1.0f,
                                  0.0f}; // bottom right corner

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Vertex position attribute x, y
    glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Texture coords attribute u, v
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          4 * sizeof(float),
                          (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // up instanced rendering
    QuadData quadDatas[100][100];
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            quadDatas[i][j].x = 2.f + i * 8.f;
            quadDatas[i][j].y = 2.f + j * 8.f;
            quadDatas[i][j].tex_h = 8.f;
            quadDatas[i][j].tex_w = 8.f;
            quadDatas[i][j].tex_x = 0.f;
            quadDatas[i][j].tex_y = 0.f;
            quadDatas[i][j].depth = 0.f;
        }
    }
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(QuadData) * 10000,
                 &quadDatas[0],
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &quadVAO_Instanced);
    glGenBuffers(1, &quadVBO_Instanced);
    glBindVertexArray(quadVAO_Instanced);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO_Instanced);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          4 * sizeof(float),
                          (void *)(2 * sizeof(float)));
    // also set instance data
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glBindBuffer(
        GL_ARRAY_BUFFER,
        instanceVBO); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(
        2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
    glVertexAttribPointer(3,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          7 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(
        2, 1); // tell OpenGL this is an instanced vertex attribute.
    glVertexAttribDivisor(3, 1);

    constexpr unsigned char quadIndices[] = {
        0,
        1,
        2, // first triangle (bottom left - top left - top right)
        0,
        2,
        3}; // second triangle (bottom left - top right - bottom right)
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 6 * sizeof(uint32_t),
                 quadIndices,
                 GL_STATIC_DRAW);

    glBindVertexArray(0);

    setupShaders();
}

jleQuadRendering::~jleQuadRendering() {
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &quadVAO);

    glDeleteBuffers(1, &quadVBO_Instanced);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteVertexArrays(1, &quadVAO_Instanced);
}

void jleQuadRendering::sendTexturedQuad(texturedQuad &texturedQuad) {
    _queuedTexturedQuads.push_back(texturedQuad);
}

void jleQuadRendering::sendTexturedHeightQuad(
    jleTexturedHeightQuad &texturedHeightQuad) {
    _queuedTexturedHeightQuads.push_back(texturedHeightQuad);
}

void jleQuadRendering::sendSimpleTexturedHeightQuad(
    jleTexturedHeightQuad &texturedHeightQuad) {
    _queuedSimpleTexturedHeightQuads.push_back(texturedHeightQuad);
}

void
jleQuadRendering::queuerender(jleFramebuffer &framebufferOut, const jleCamera &camera)
{
    render(framebufferOut,
           camera,
           _queuedTexturedQuads,
           _queuedTexturedHeightQuads,
           _queuedSimpleTexturedHeightQuads,
           true);
}

void jleQuadRendering::clearBuffersForNextFrame() {
    _queuedTexturedQuads.clear();
    _queuedTexturedHeightQuads.clear();
    _queuedSimpleTexturedHeightQuads.clear();
}

void jleQuadRendering::render(jleFramebuffer &framebufferOut,
                         const jleCamera &camera,
                         const std::vector<texturedQuad> &texturedQuads,
                         const std::vector<jleTexturedHeightQuad> &texturedHeightQuads,
                         const std::vector<jleTexturedHeightQuad> &texturedSimpleHeightQuads,
                         bool clearDepthColor)
{

    JLE_SCOPE_PROFILE(jleQuadRendering::Render)

    const int viewportWidth = framebufferOut.width();
    const int viewportHeight = framebufferOut.height();

    glm::mat4 view = camera.getProjectionViewMatrix();

    framebufferOut.bind();

    if (clearDepthColor) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Make everything black
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    glEnable(GL_DEPTH_TEST);

    // Change viewport dimensions to match framebuffer's dimensions
    glViewport(0, 0, viewportWidth, viewportHeight);

    processTexturedQuads(texturedQuads, view);

    processTexturedHeightQuads(texturedHeightQuads, view, camera.getViewPosition());

    processSimpleTexturedHeightQuads(texturedSimpleHeightQuads, view, camera.getViewPosition());

    renderShadowCubes(camera.getProjectionViewMatrix());

    framebufferOut.bindDefault();
}

void jleQuadRendering::processTexturedQuads(
    const std::vector<texturedQuad> &texturedQuads, glm::mat4 &view) {
    JLE_SCOPE_PROFILE(jleQuadRendering::processTexturedQuads)

    std::unordered_map<std::shared_ptr<jleTexture>, std::vector<QuadData>>
        quadDataMap;

    for (auto &&quad : texturedQuads) {
        QuadData qd;
        qd.depth = quad.depth;
        qd.tex_h = quad.height;
        qd.tex_w = quad.width;
        qd.tex_x = quad.textureX;
        qd.tex_y = quad.textureY;
        qd.x = quad.x;
        qd.y = quad.y;
        auto &vec = quadDataMap[quad.texture];
        vec.push_back(qd);
    }

    quadShaderInstanced.use();
    quadShaderInstanced.SetMat4("camera", view);

    for (auto &&key : quadDataMap) {

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(QuadData) * key.second.size(),
                     &key.second[0],
                     GL_STATIC_DRAW);
        glVertexAttribPointer(
            2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
        glVertexAttribPointer(3,
                              4,
                              GL_FLOAT,
                              GL_FALSE,
                              7 * sizeof(float),
                              (void *)(3 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        quadShaderInstanced.use();

        if (!key.first->isActive()) {
            key.first->setActive();
            quadShaderInstanced.SetVec2("textureDims",
                                        glm::vec2{float(key.first->width()),
                                                  float(key.first->height())});
            quadShaderInstanced.SetInt("texture0", 0);
        }

        glBindVertexArray(quadVAO_Instanced);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glDrawElementsInstanced(
            GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (void *)0, key.second.size());
        glBindVertexArray(0);
    }
}

void jleQuadRendering::processTexturedHeightQuads(
    const std::vector<jleTexturedHeightQuad> &texturedHeightQuads,
    glm::mat4 &view,
    glm::vec3 viewPos) {
    JLE_SCOPE_PROFILE(jleQuadRendering::processTexturedHeightQuads)

    std::unordered_map<std::shared_ptr<TextureWithHeightmap>,
                       std::vector<QuadData>>
        quadDataMap;

    static const glm::vec3 cameraPositionPixels{0.f, 500.f, 500.f};
    viewPos += cameraPositionPixels;

    for (auto &&quad : texturedHeightQuads) {
        QuadData qd;
        qd.depth = quad.depth;
        qd.tex_h = quad.height;
        qd.tex_w = quad.width;
        qd.tex_x = quad.textureX;
        qd.tex_y = quad.textureY;
        qd.x = quad.x;
        qd.y = quad.y;
        auto &vec = quadDataMap[quad.mtextureWithHeightmap];
        vec.push_back(qd);
    }

    quadHeightmapShaderInstanced.use();
    quadHeightmapShaderInstanced.SetMat4("camera", view);
    quadHeightmapShaderInstanced.SetVec3("viewPos", viewPos);
    quadHeightmapShaderInstanced.SetVec3("light.position", lightPos);

    for (auto &&key : quadDataMap) {

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(QuadData) * key.second.size(),
                     &key.second[0],
                     GL_STATIC_DRAW);
        glVertexAttribPointer(
            2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
        glVertexAttribPointer(3,
                              4,
                              GL_FLOAT,
                              GL_FALSE,
                              7 * sizeof(float),
                              (void *)(3 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        if (!key.first->texture->isActive()) {
            key.first->texture->setActive(0);
            key.first->heightmap->setActive(1);
            key.first->normalmap->setActive(2);
            quadHeightmapShaderInstanced.SetVec2(
                "textureDims",
                glm::vec2{float(key.first->texture->width()),
                          float(key.first->texture->height())});

            quadHeightmapShaderInstanced.SetInt("texture_albedo", 0);
            quadHeightmapShaderInstanced.SetInt("texture_heightmap", 1);
            quadHeightmapShaderInstanced.SetInt("texture_normal", 2);
        }

        glBindVertexArray(quadVAO_Instanced);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glDrawElementsInstanced(
            GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (void *)0, key.second.size());
        glBindVertexArray(0);
    }
}

void jleQuadRendering::processSimpleTexturedHeightQuads(
    const std::vector<jleTexturedHeightQuad> &texturedHeightQuads,
    glm::mat4 &view,
    glm::vec3 viewPos) {
    JLE_SCOPE_PROFILE(jleQuadRendering::processSimpleTexturedHeightQuads)

    std::unordered_map<std::shared_ptr<TextureWithHeightmap>,
                       std::vector<QuadData>>
        quadDataMap;

    static const glm::vec3 cameraPositionPixels{0.f, 500.f, 500.f};
    viewPos += cameraPositionPixels;

    for (auto &&quad : texturedHeightQuads) {
        QuadData qd;
        qd.depth = quad.depth;
        qd.tex_h = quad.height;
        qd.tex_w = quad.width;
        qd.tex_x = quad.textureX;
        qd.tex_y = quad.textureY;
        qd.x = quad.x;
        qd.y = quad.y;
        auto &vec = quadDataMap[quad.mtextureWithHeightmap];
        vec.push_back(qd);
    }

    quadHeightmapShaderInstancedSimple.use();
    quadHeightmapShaderInstancedSimple.SetMat4("camera", view);
    quadHeightmapShaderInstancedSimple.SetVec3("viewPos", viewPos);

    for (auto &&key : quadDataMap) {

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(QuadData) * key.second.size(),
                     &key.second[0],
                     GL_STATIC_DRAW);
        glVertexAttribPointer(
            2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
        glVertexAttribPointer(3,
                              4,
                              GL_FLOAT,
                              GL_FALSE,
                              7 * sizeof(float),
                              (void *)(3 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        if (!key.first->texture->isActive()) {
            key.first->texture->setActive(0);
            key.first->heightmap->setActive(1);
            quadHeightmapShaderInstancedSimple.SetVec2(
                "textureDims",
                glm::vec2{float(key.first->texture->width()),
                          float(key.first->texture->height())});

            quadHeightmapShaderInstancedSimple.SetInt("texture_albedo", 0);
            quadHeightmapShaderInstancedSimple.SetInt("texture_heightmap", 1);
        }

        glBindVertexArray(quadVAO_Instanced);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glDrawElementsInstanced(
            GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (void *)0, key.second.size());
        glBindVertexArray(0);
    }
}

void jleQuadRendering::setupShaders() {

    // Set up the angles on the "camera", as it was rendered in 3D software
    static const float xyAngle = 0.f;
    static const float zAngle = 90 - 60.f;
    static const float cosXY = cos(xyAngle * glm::pi<float>() / 180.0);
    static const float sinXY = sin(xyAngle * glm::pi<float>() / 180.0);
    static const float cosZ = cos(zAngle * glm::pi<float>() / 180.0);
    static const float sinZ = sin(zAngle * glm::pi<float>() / 180.0);

    static const glm::mat3x3 cartToIso{
        cosXY, sinXY / sinZ, 0, -sinXY, cosXY / sinZ, 0, 0, 0, 0};

    static const glm::mat3x3 isoToCart{
        cosXY, -sinXY, 0, sinXY * sinZ, cosXY * sinZ, -cosZ, 0, 0, 0};

    // Magic Height Factor is calculated from essentially testing different
    // values. It is used to find the height (z value) of a pixel from the
    // height map texture. Different scaling on the 2.5D assets will impact this
    // value, and it needs modifying.
    static const float magicHeightFactor = 127.f;

    quadHeightmapShaderInstanced.use();

    quadHeightmapShaderInstanced.SetFloat("sinZ", sinZ);
    quadHeightmapShaderInstanced.SetFloat("sinZ_inverse", 1.f / sinZ);
    quadHeightmapShaderInstanced.SetFloat("magicHeightFactor",
                                          magicHeightFactor);
    quadHeightmapShaderInstanced.SetMat3("cartToIso", cartToIso);
    quadHeightmapShaderInstanced.SetBool("gamma", true);

    quadHeightmapShaderInstanced.SetVec3("light.ambient", 0.8f, 0.8f, 0.8f);
    quadHeightmapShaderInstanced.SetVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
    quadHeightmapShaderInstanced.SetVec3("light.specular", 1.0f, 1.0f, 1.0f);
    quadHeightmapShaderInstanced.SetFloat("light.constant", 1.0f);
    quadHeightmapShaderInstanced.SetFloat("light.linear", 0.0009f);
    quadHeightmapShaderInstanced.SetFloat("light.quadratic", 0.00032f);

    quadHeightmapShaderInstancedSimple.SetFloat("sinZ", sinZ);
    quadHeightmapShaderInstancedSimple.SetFloat("sinZ_inverse", 1.f / sinZ);
    quadHeightmapShaderInstancedSimple.SetFloat("magicHeightFactor",
                                                magicHeightFactor);
    quadHeightmapShaderInstancedSimple.SetMat3("cartToIso", cartToIso);
}

void jleQuadRendering::renderCube(glm::mat4 &model, jleShader &shader) {
    static unsigned int cubeVAO = 0;
    static unsigned int cubeVBO = 0;

    // initialize (if necessary)
    if (cubeVAO == 0) {
        float vertices[] = {
            // back face
            -1.0f,
            -1.0f,
            -1.0f,
            0.0f,
            0.0f,
            -1.0f,
            0.0f,
            0.0f, // bottom-left
            1.0f,
            1.0f,
            -1.0f,
            0.0f,
            0.0f,
            -1.0f,
            1.0f,
            1.0f, // top-right
            1.0f,
            -1.0f,
            -1.0f,
            0.0f,
            0.0f,
            -1.0f,
            1.0f,
            0.0f, // bottom-right
            1.0f,
            1.0f,
            -1.0f,
            0.0f,
            0.0f,
            -1.0f,
            1.0f,
            1.0f, // top-right
            -1.0f,
            -1.0f,
            -1.0f,
            0.0f,
            0.0f,
            -1.0f,
            0.0f,
            0.0f, // bottom-left
            -1.0f,
            1.0f,
            -1.0f,
            0.0f,
            0.0f,
            -1.0f,
            0.0f,
            1.0f, // top-left
            // front face
            -1.0f,
            -1.0f,
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            0.0f,
            0.0f, // bottom-left
            1.0f,
            -1.0f,
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            1.0f,
            0.0f, // bottom-right
            1.0f,
            1.0f,
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            1.0f,
            1.0f, // top-right
            1.0f,
            1.0f,
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            1.0f,
            1.0f, // top-right
            -1.0f,
            1.0f,
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            0.0f,
            1.0f, // top-left
            -1.0f,
            -1.0f,
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            0.0f,
            0.0f, // bottom-left
            // left face
            -1.0f,
            1.0f,
            1.0f,
            -1.0f,
            0.0f,
            0.0f,
            1.0f,
            0.0f, // top-right
            -1.0f,
            1.0f,
            -1.0f,
            -1.0f,
            0.0f,
            0.0f,
            1.0f,
            1.0f, // top-left
            -1.0f,
            -1.0f,
            -1.0f,
            -1.0f,
            0.0f,
            0.0f,
            0.0f,
            1.0f, // bottom-left
            -1.0f,
            -1.0f,
            -1.0f,
            -1.0f,
            0.0f,
            0.0f,
            0.0f,
            1.0f, // bottom-left
            -1.0f,
            -1.0f,
            1.0f,
            -1.0f,
            0.0f,
            0.0f,
            0.0f,
            0.0f, // bottom-right
            -1.0f,
            1.0f,
            1.0f,
            -1.0f,
            0.0f,
            0.0f,
            1.0f,
            0.0f, // top-right
            // right face
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            0.0f, // top-left
            1.0f,
            -1.0f,
            -1.0f,
            1.0f,
            0.0f,
            0.0f,
            0.0f,
            1.0f, // bottom-right
            1.0f,
            1.0f,
            -1.0f,
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            1.0f, // top-right
            1.0f,
            -1.0f,
            -1.0f,
            1.0f,
            0.0f,
            0.0f,
            0.0f,
            1.0f, // bottom-right
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            0.0f, // top-left
            1.0f,
            -1.0f,
            1.0f,
            1.0f,
            0.0f,
            0.0f,
            0.0f,
            0.0f, // bottom-left
            // bottom face
            -1.0f,
            -1.0f,
            -1.0f,
            0.0f,
            -1.0f,
            0.0f,
            0.0f,
            1.0f, // top-right
            1.0f,
            -1.0f,
            -1.0f,
            0.0f,
            -1.0f,
            0.0f,
            1.0f,
            1.0f, // top-left
            1.0f,
            -1.0f,
            1.0f,
            0.0f,
            -1.0f,
            0.0f,
            1.0f,
            0.0f, // bottom-left
            1.0f,
            -1.0f,
            1.0f,
            0.0f,
            -1.0f,
            0.0f,
            1.0f,
            0.0f, // bottom-left
            -1.0f,
            -1.0f,
            1.0f,
            0.0f,
            -1.0f,
            0.0f,
            0.0f,
            0.0f, // bottom-right
            -1.0f,
            -1.0f,
            -1.0f,
            0.0f,
            -1.0f,
            0.0f,
            0.0f,
            1.0f, // top-right
            // top face
            -1.0f,
            1.0f,
            -1.0f,
            0.0f,
            1.0f,
            0.0f,
            0.0f,
            1.0f, // top-left
            1.0f,
            1.0f,
            1.0f,
            0.0f,
            1.0f,
            0.0f,
            1.0f,
            0.0f, // bottom-right
            1.0f,
            1.0f,
            -1.0f,
            0.0f,
            1.0f,
            0.0f,
            1.0f,
            1.0f, // top-right
            1.0f,
            1.0f,
            1.0f,
            0.0f,
            1.0f,
            0.0f,
            1.0f,
            0.0f, // bottom-right
            -1.0f,
            1.0f,
            -1.0f,
            0.0f,
            1.0f,
            0.0f,
            0.0f,
            1.0f, // top-left
            -1.0f,
            1.0f,
            1.0f,
            0.0f,
            1.0f,
            0.0f,
            0.0f,
            0.0f // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(
            GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              8 * sizeof(float),
                              (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              8 * sizeof(float),
                              (void *)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    shader.use();
    shader.SetMat4("model", model);

    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void
jleQuadRendering::renderShadowCubes(const glm::mat4 &view)
{

    std::random_device rd;                              // obtain a random number from hardware
    std::mt19937 gen(rd());                             // seed the generator
    std::uniform_int_distribution<> distr(-30.f, 30.f); // define the range

    /* glm::mat4 lightProjection, lightView;
     glm::mat4 lightSpaceMatrix;
     float near_plane = 1.0f, far_plane = 7.5f;
     lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane,
     far_plane); lightView = glm::lookAt(lightPos, glm::vec3(0.0f),
     glm::vec3(0.0, 1.0, 0.0)); lightSpaceMatrix = lightProjection * lightView;
     // render scene from light's point of view*/

    shadowMappingShader.use();
    shadowMappingShader.SetMat4("lightSpaceMatrix", view);

    // static const float zAngle = 90.f - 35.24f;
    // static const float sinZ = sin(zAngle * glm::pi<float>() / 180.0);

    glm::mat4 model = glm::mat4(1.0f);
    auto vec3 = lightPos;
    // vec3.z = lightPos.y;
    // model = glm::rotate(model, glm::radians(90-35.24f), glm::vec3{1.f, 0.f,
    // 0.f});
    model = glm::translate(model, vec3);
    model = glm::rotate(model, glm::radians(45.f), glm::normalize(glm::vec3(0.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(5.f));

    renderCube(model, shadowMappingShader);

    static std::vector<glm::mat4> models;
    static bool generatedModels = false;
    if (!generatedModels) {
        for (int i = 0; i < 45; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(
                model, glm::vec3{distr(gen), distr(gen), distr(gen)});
            model = glm::scale(model, glm::vec3(1.f));
            models.push_back(model);
            generatedModels = true;
        }
    }

    for (auto &&model : models) {
        renderCube(model, shadowMappingShader);
    }
}
