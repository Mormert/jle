// Copyright (c) 2022. Johan Lind

#include "QuadRendering_OpenGL.h"
#include "jlePathDefines.h"
#include "plog/Log.h"
#include "jleProfiler.h"
#include <thread>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <GLES3/gl3.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else

#include <glad/glad.h>

#endif

#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <unordered_map>
#include <iostream>

namespace jle {

    struct QuadData {
        float x, y, depth;
        float tex_x, tex_y, tex_w, tex_h;
    };


    QuadRendering_OpenGL::QuadRendering_OpenGL()
            : quadShader{std::string{JLE_ENGINE_PATH_SHADERS + "/quad.vert"}.c_str(),
                         std::string{JLE_ENGINE_PATH_SHADERS + "/quad.frag"}.c_str()},
              quadShaderInstanced{std::string{JLE_ENGINE_PATH_SHADERS + "/quadInstanced.vert"}.c_str(),
                                  std::string{JLE_ENGINE_PATH_SHADERS + "/quadInstanced.frag"}.c_str()},
              quadHeightmapShaderInstanced{
                      std::string{JLE_ENGINE_PATH_SHADERS + "/quadHeightmapInstanced.vert"}.c_str(),
                      std::string{JLE_ENGINE_PATH_SHADERS + "/quadHeightmapInstanced.frag"}.c_str()} {

        LOG_VERBOSE << "Constructing OpenGL Quad Rendering";

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);

        // x, y, u, v
        constexpr float vertices[] = {
                0.0f, 0.0f, 0.0f, 0.0f,        // bottom left corner
                0.0f, 1.0f, 0.0f, 1.0f,        // top left corner
                1.0f, 1.0f, 1.0f, 1.0f,        // top right corner
                1.0f, 0.0f, 1.0f, 0.0f};        // bottom right corner

        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Vertex position attribute x, y
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);

        // Texture coords attribute u, v
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
        glEnableVertexAttribArray(1);


        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);


        // Setup instanced rendering
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(QuadData) * 10000, &quadDatas[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenVertexArrays(1, &quadVAO_Instanced);
        glGenBuffers(1, &quadVBO_Instanced);
        glBindVertexArray(quadVAO_Instanced);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO_Instanced);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
        // also set instance data
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *) 0);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *) (3 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.
        glVertexAttribDivisor(3, 1);

        constexpr unsigned char quadIndices[] = {0, 1, 2, // first triangle (bottom left - top left - top right)
                                                 0, 2, 3}; // second triangle (bottom left - top right - bottom right)
        glGenBuffers(1, &elementbuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), quadIndices, GL_STATIC_DRAW);

        glBindVertexArray(0);

        SetupShaders();

    }

    QuadRendering_OpenGL::~QuadRendering_OpenGL() {
        glDeleteBuffers(1, &quadVBO);
        glDeleteVertexArrays(1, &quadVAO);

        glDeleteBuffers(1, &quadVBO_Instanced);
        glDeleteBuffers(1, &elementbuffer);
        glDeleteVertexArrays(1, &quadVAO_Instanced);
    }

    void QuadRendering_OpenGL::SendTexturedQuad(TexturedQuad &texturedQuad, RenderingMethod renderingMethod) {
        mQueuedTexturedQuads.push_back(texturedQuad);
    }

    void QuadRendering_OpenGL::SendTexturedHeightQuad(TexturedHeightQuad &texturedHeightQuad,
                                                      RenderingMethod renderingMethod) {
        mQueuedTexturedHeightQuads.push_back(texturedHeightQuad);
    }

    void QuadRendering_OpenGL::SendColoredQuad(ColoredQuad &coloredQuad, RenderingMethod renderingMethod) {

    }

    void QuadRendering_OpenGL::QueueRender(iFramebuffer &framebufferOut, const jleCamera &camera) {
        Render(framebufferOut, camera, mQueuedTexturedQuads, mQueuedTexturedHeightQuads, true);
    }

    void QuadRendering_OpenGL::ClearBuffersForNextFrame() {
        mQueuedTexturedQuads.clear();
        mQueuedTexturedHeightQuads.clear();
    }

    void QuadRendering_OpenGL::Render(iFramebuffer &framebufferOut, const jleCamera &camera,
                                      const std::vector<TexturedQuad> &texturedQuads,
                                      const std::vector<TexturedHeightQuad> &texturedHeightQuads,
                                      bool clearDepthColor) {

        JLE_SCOPE_PROFILE(QuadRendering_OpenGL::Render)

        const int viewportWidth = framebufferOut.GetWidth();
        const int viewportHeight = framebufferOut.GetHeight();

        glm::mat4 view{1.f};
        view = glm::ortho(static_cast<float>(camera.GetIntX()),
                          static_cast<float>(camera.GetIntX() + viewportWidth),
                          static_cast<float>(camera.GetIntY() + viewportHeight),
                          static_cast<float>(camera.GetIntY()), -1000.f, 1000.f);

        framebufferOut.BindToFramebuffer();

        if (clearDepthColor) {
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Make everything black
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        glEnable(GL_DEPTH_TEST);

        // Change viewport dimensions to match framebuffer's dimensions
        glViewport(0, 0, viewportWidth, viewportHeight);

        ProcessTexturedQuads(texturedQuads, view);
        ProcessTexturedHeightQuads(texturedHeightQuads, view, glm::vec3{camera.mXNoOffset, camera.mYNoOffset, 0.f});

        framebufferOut.BindToDefaultFramebuffer();
    }

    void QuadRendering_OpenGL::ProcessTexturedQuads(const std::vector<TexturedQuad> &texturedQuads, glm::mat4 &view) {
        JLE_SCOPE_PROFILE(QuadRendering_OpenGL::ProcessTexturedQuads)

        std::unordered_map<std::shared_ptr<iTexture>, std::vector<QuadData>> quadDataMap;


        for (auto &&quad: texturedQuads) {
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

        quadShaderInstanced.Use();
        quadShaderInstanced.SetMat4("camera", view);


        for (auto &&key: quadDataMap) {

            glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(QuadData) * key.second.size(), &key.second[0], GL_STATIC_DRAW);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *) 0);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *) (3 * sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);


            quadShaderInstanced.Use();

            if (!key.first->IsActive()) {
                key.first->SetToActiveTexture();
                quadShaderInstanced.SetVec2("textureDims",
                                            glm::vec2{float(key.first->GetWidth()), float(key.first->GetHeight())});
                quadShaderInstanced.SetInt("texture0", 0);

            }

            glBindVertexArray(quadVAO_Instanced);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (void *) 0, key.second.size());
            glBindVertexArray(0);
        }
    }

    void QuadRendering_OpenGL::ProcessTexturedHeightQuads(const std::vector<TexturedHeightQuad> &texturedHeightQuads,
                                                          glm::mat4 &view, glm::vec3 viewPos) {
        JLE_SCOPE_PROFILE(QuadRendering_OpenGL::ProcessTexturedHeightQuads)

        std::unordered_map<std::shared_ptr<TextureWithHeightmap>, std::vector<QuadData>> quadDataMap;

        static const glm::vec3 cameraPositionPixels{0.f, 500.f, 500.f};
        viewPos += cameraPositionPixels;

        for (auto &&quad: texturedHeightQuads) {
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

        quadHeightmapShaderInstanced.Use();
        quadHeightmapShaderInstanced.SetMat4("camera", view);
        quadHeightmapShaderInstanced.SetVec3("viewPos", viewPos);
        quadHeightmapShaderInstanced.SetVec3("light.position", lightPos);

        for (auto &&key: quadDataMap) {

            glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(QuadData) * key.second.size(), &key.second[0], GL_STATIC_DRAW);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *) 0);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *) (3 * sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            if (!key.first->texture->IsActive()) {
                key.first->texture->SetToActiveTexture(0);
                key.first->heightmap->SetToActiveTexture(1);
                key.first->normalmap->SetToActiveTexture(2);
                quadHeightmapShaderInstanced.SetVec2("textureDims",
                                                     glm::vec2{float(key.first->texture->GetWidth()),
                                                               float(key.first->texture->GetHeight())});

                quadHeightmapShaderInstanced.SetInt("texture_albedo", 0);
                quadHeightmapShaderInstanced.SetInt("texture_heightmap", 1);
                quadHeightmapShaderInstanced.SetInt("texture_normal", 2);
            }

            glBindVertexArray(quadVAO_Instanced);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (void *) 0, key.second.size());
            glBindVertexArray(0);
        }
    }

    void QuadRendering_OpenGL::SetupShaders() {

        // Set up the angles on the "camera", as it was rendered in 3D software
        static const float xyAngle = 0.f;
        static const float zAngle = 90.f - 35.24f;
        static const float cosXY = cos(xyAngle * glm::pi<float>() / 180.0);
        static const float sinXY = sin(xyAngle * glm::pi<float>() / 180.0);
        static const float cosZ = cos(zAngle * glm::pi<float>() / 180.0);
        static const float sinZ = sin(zAngle * glm::pi<float>() / 180.0);

        static const glm::mat3x3 cartToIso{cosXY, sinXY / sinZ, 0,
                                           -sinXY, cosXY / sinZ, 0,
                                           0, 0, 0};

        static const glm::mat3x3 isoToCart{cosXY, -sinXY, 0,
                                           sinXY * sinZ, cosXY * sinZ, -cosZ,
                                           0, 0, 0};

        // Magic Height Factor is calculated from essentially testing different values.
        // It is used to find the height (z value) of a pixel from the height map texture.
        // Different scaling on the 2.5D assets will impact this value, and it needs modifying.
        static const float magicHeightFactor = 127.f;

        quadHeightmapShaderInstanced.Use();

        quadHeightmapShaderInstanced.SetFloat("sinZ", sinZ);
        quadHeightmapShaderInstanced.SetFloat("sinZ_inverse", 1.f / sinZ);
        quadHeightmapShaderInstanced.SetFloat("magicHeightFactor", magicHeightFactor);
        quadHeightmapShaderInstanced.SetMat3("cartToIso", cartToIso);
        quadHeightmapShaderInstanced.SetBool("gamma", true);

        quadHeightmapShaderInstanced.SetVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        quadHeightmapShaderInstanced.SetVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
        quadHeightmapShaderInstanced.SetVec3("light.specular", 1.0f, 1.0f, 1.0f);
        quadHeightmapShaderInstanced.SetFloat("light.constant", 1.0f);
        quadHeightmapShaderInstanced.SetFloat("light.linear", 0.0009f);
        quadHeightmapShaderInstanced.SetFloat("light.quadratic", 0.00032f);
    }

}
