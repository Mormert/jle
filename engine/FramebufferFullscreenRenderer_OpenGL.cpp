// Copyright (c) 2022. Johan Lind

#include "FramebufferFullscreenRenderer_OpenGL.h"

#include "3rdparty/glad/glad.h"
#include "GLState.h"
#include "jlePathDefines.h"

namespace jle {
    constexpr float quadVertices[] = { // Vertex attributes for a quad that fills the entire screen in NDC
            // positions   // texCoords
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f
    };

    FramebufferFullscreenRenderer_OpenGL::~FramebufferFullscreenRenderer_OpenGL() {
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
    }

    FramebufferFullscreenRenderer_OpenGL::FramebufferFullscreenRenderer_OpenGL() :
            quadScreenShader{std::string{JLE_ENGINE_PATH + "EngineResources/quadScreen.vert"}.c_str(),
                             std::string{JLE_ENGINE_PATH + "EngineResources/quadScreen.frag"}.c_str()} {
        // Configure screen quad
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
    }

    void FramebufferFullscreenRenderer_OpenGL::RenderFramebufferFullscreen(iFramebuffer &framebuffer,
                                                                           unsigned int screenWidth,
                                                                           unsigned int screenHeight) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default framebuffer
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, screenWidth, screenHeight);

        const auto framebufferTexture = framebuffer.GetTexture();

        quadScreenShader.Use();
        glBindVertexArray(quadVAO);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, framebufferTexture);
        jle::GLState::globalActiveTexture = framebufferTexture;
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}
