// Copyright (c) 2022. Johan Lind

#pragma once

#include <glm/fwd.hpp>
#include <jleCamera.h>
#include <jleFrameBuffer.h>
#include <jleShader.h>
#include <vector>

class jle3DRenderer
{
public:
    jle3DRenderer();

    virtual ~jle3DRenderer();

    void queuerender(jleFramebuffer &framebufferOut, const jleCamera &camera);

    void render(jleFramebuffer &framebufferOut, const jleCamera &camera, const std::vector<glm::mat4> &cubeTransforms);

    void sendExampleCube(const glm::mat4 &transform);

    void clearBuffersForNextFrame();

private:
    void renderExampleCubes(const jleCamera &camera, const std::vector<glm::mat4> &cubeTransforms);

    std::vector<glm::mat4> _queuedExampleCubes;
    jleShader _exampleCubeShader;
    unsigned int _exampleCubeVBO{}, _exampleCubeVAO{}, _exampleCubeInstanceBuffer{};
};
