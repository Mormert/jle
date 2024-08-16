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

#include "jleFullscreenRendering.h"

#include "jleIncludeGL.h"

#include "jleFrameBufferInterface.h"

constexpr float quadVertices[] =
    { // Vertex attributes for a quad that fills the entire screen in NDC
      // positions   // texCoords
        -1.0f, 1.0f, 0.0f, 1.0f,  -1.0f, -1.0f,
        0.0f,  0.0f, 1.0f, -1.0f, 1.0f,  0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  -1.0f,
        1.0f,  0.0f, 1.0f, 1.0f,  1.0f,  1.0f};

jleFullscreenRendering::jleFullscreenRendering()
    : quadScreenShader{jlePath{"ER:shaders/quadScreen.glsl"}} {
    // Configure screen quad
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          4 * sizeof(float),
                          (void *)(2 * sizeof(float)));
}

jleFullscreenRendering::~jleFullscreenRendering() {
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}

void
jleFullscreenRendering::renderFramebufferFullscreen(jleFramebufferInterface &framebuffer,
                                                    unsigned int screenWidth,
                                                    unsigned int screenHeight)
{
    // Back to default screen framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set viewport to cover the entire screen
    glViewport(0, 0, screenWidth, screenHeight);

    quadScreenShader->use();
    glBindVertexArray(quadVAO);

    // Disable depth testing for the fullscreen quad
    glDisable(GL_DEPTH_TEST);

    // Get the texture from the framebuffer
    glBindTexture(GL_TEXTURE_2D, framebuffer.texture());

    // Draw quad with framebuffer's texture over the entire screen
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
