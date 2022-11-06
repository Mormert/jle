// Copyright (c) 2022. Johan Lind

#include "jle3DRenderer.h"

#include "jleCamera.h"
#include "jleFrameBuffer.h"
#include "jleGameEngine.h"
#include "jlePathDefines.h"
#include "jleProfiler.h"
#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <random>

jle3DRenderer::jle3DRenderer()
    : _exampleCubeShader{std::string{JLE_ENGINE_PATH_SHADERS + "/exampleCube.vert"}.c_str(),
                         std::string{JLE_ENGINE_PATH_SHADERS + "/exampleCube.frag"}.c_str()}
{

    constexpr float exampleCubeData[] = {
        // clang-format off
    // Vertex position XYZ,		        Color RGB
    -0.5f, -0.5f, -0.5f,            1.0f, 1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,             0.0f, 1.0f, 1.0f,
    0.5f, 0.5f, -0.5f,          0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f,          0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f,         1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,        1.0f, 1.0f, 1.0f,

    -0.5f, -0.5f, 0.5f,         1.0f, 1.0f, 0.5f,
    0.5f, -0.5f, 0.5f,          0.0f, 1.0f, 0.5f,
    0.5f, 0.5f, 0.5f,           0.0f, 0.0f, 0.5f,
    0.5f, 0.5f, 0.5f,           0.0f, 0.0f, 0.5f,
    -0.5f, 0.5f, 0.5f,          1.0f, 0.0f, 0.5f,
    -0.5f, -0.5f, 0.5f,         1.0f, 1.0f, 0.5f,

    -0.5f, 0.5f, 0.5f,          0.0f, 0.0f, 0.7f,
    -0.5f, 0.5f, -0.5f,         1.0f, 0.0f, 0.7f,
    -0.5f, -0.5f, -0.5f,        1.0f, 1.0f, 0.7f,
    -0.5f, -0.5f, -0.5f,        1.0f, 1.0f, 0.7f,
    -0.5f, -0.5f, 0.5f,         0.0f, 1.0f, 0.7f,
    -0.5f, 0.5f, 0.5f,          0.0f, 0.0f, 0.7f,

    0.5f, 0.5f, 0.5f,           0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f,          1.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,         1.0f, 1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,         1.0f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f,          0.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f,           0.0f, 0.0f, 1.0f,

    -0.5f, -0.5f, -0.5f,        0.0f, 0.0f, 0.2f,
    0.5f, -0.5f, -0.5f,         1.0f, 0.0f, 0.2f,
    0.5f, -0.5f, 0.5f,          1.0f, 1.0f, 0.2f,
    0.5f, -0.5f, 0.5f,          1.0f, 1.0f, 0.2f,
    -0.5f, -0.5f, 0.5f,         0.0f, 1.0f, 0.2f,
    -0.5f, -0.5f, -0.5f,        0.0f, 0.0f, 0.2f,

    -0.5f, 0.5f, -0.5f,         0.0f, 0.0f, 0.4f,
    0.5f, 0.5f, -0.5f,          1.0f, 0.0f, 0.4f,
    0.5f, 0.5f, 0.5f,           1.0f, 1.0f, 0.4f,
    0.5f, 0.5f, 0.5f,           1.0f, 1.0f, 0.4f,
    -0.5f, 0.5f, 0.5f,          0.0f, 1.0f, 0.4f,
    -0.5f, 0.5f, -0.5f,         0.0f, 0.0f, 0.4f
        // clang-format on
    };

    // Setup vertex array object
    glGenVertexArrays(1, &_exampleCubeVAO);
    glGenBuffers(1, &_exampleCubeVBO);

    glBindVertexArray(_exampleCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, _exampleCubeVBO);

    // Buffer one cube's data (XYZ pos, RGB vertex colors)
    glBufferData(GL_ARRAY_BUFFER, sizeof(exampleCubeData), exampleCubeData, GL_STATIC_DRAW);

    // Vertex position attribute x, y, z
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Vertex color attribute r, g, b
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &_exampleCubeInstanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _exampleCubeInstanceBuffer);

    // Instance the transform data (glm::mat4)
    auto vec4Size = sizeof(glm::vec4);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)0);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(1 * vec4Size));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(2 * vec4Size));
    glEnableVertexAttribArray(4);

    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(3 * vec4Size));
    glEnableVertexAttribArray(5);

    // Only go to next attribute on next instance draw for the transform matrix
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);

    glBindVertexArray(0);
}

jle3DRenderer::~jle3DRenderer()
{
    glDeleteBuffers(1, &_exampleCubeVBO);
    glDeleteBuffers(1, &_exampleCubeInstanceBuffer);
    glDeleteVertexArrays(1, &_exampleCubeVAO);
}

void
jle3DRenderer::queuerender(jleFramebuffer &framebufferOut, const jleCamera &camera)
{
    render(framebufferOut, camera, _queuedExampleCubes);
}

void
jle3DRenderer::render(jleFramebuffer &framebufferOut,
                      const jleCamera &camera,
                      const std::vector<glm::mat4> &cubeTransforms)
{
    JLE_SCOPE_PROFILE(jle3DRenderer::render);

    const int viewportWidth = framebufferOut.width();
    const int viewportHeight = framebufferOut.height();

    framebufferOut.bind();

    glEnable(GL_DEPTH_TEST);

    // Change viewport dimensions to match framebuffer's dimensions
    glViewport(0, 0, viewportWidth, viewportHeight);

    renderExampleCubes(camera, cubeTransforms);

    framebufferOut.bindDefault();
}

void
jle3DRenderer::sendExampleCube(const glm::mat4 &transform)
{
    _queuedExampleCubes.push_back(transform);
}

void
jle3DRenderer::clearBuffersForNextFrame()
{
    _queuedExampleCubes.clear();
}

void
jle3DRenderer::renderExampleCubes(const jleCamera &camera, const std::vector<glm::mat4> &cubeTransforms)
{
    if (cubeTransforms.empty()) {
        return;
    }

    _exampleCubeShader.use();
    _exampleCubeShader.SetMat4("projView", camera.getProjectionViewMatrix());

    glBindVertexArray(_exampleCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, _exampleCubeInstanceBuffer);
    glBufferData(GL_ARRAY_BUFFER, cubeTransforms.size() * sizeof(glm::mat4), &cubeTransforms[0], GL_DYNAMIC_DRAW);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, cubeTransforms.size());
    glBindVertexArray(0);
}
