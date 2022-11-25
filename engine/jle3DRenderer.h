// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleSkybox.h"
#include <glm/fwd.hpp>
#include <jleCamera.h>
#include <jleFrameBuffer.h>
#include <jleMesh.h>
#include <jleShader.h>
#include <memory>
#include <vector>

class jle3DRenderer
{
public:
    struct jle3DRendererQueuedMesh {
        glm::mat4 transform;
        std::shared_ptr<jleMesh> mesh;
        int instanceId;
    };

    struct jle3DRendererLight {
        glm::vec3 position;
        glm::vec3 color;
    };

    jle3DRenderer();

    virtual ~jle3DRenderer();

    void queuerender(jleFramebuffer &framebufferOut, const jleCamera &camera);

    void render(jleFramebuffer &framebufferOut,
                const jleCamera &camera,
                const std::vector<glm::mat4> &cubeTransforms,
                const std::vector<jle3DRendererQueuedMesh> &meshes);

    void renderMeshesPicking(jleFramebuffer &framebufferOut, const jleCamera &camera);

    void sendExampleCube(const glm::mat4 &transform);

    void sendMesh(const std::shared_ptr<jleMesh> &mesh, const glm::mat4 &transform, int instanceId);

    void sendLight(const glm::vec3 &position, const glm::vec3 &color);

    void setSkybox(const std::shared_ptr<jleSkybox> &skybox);

    void clearBuffersForNextFrame();

private:
    void renderFullscreenQuad();

    void renderExampleCubes(const jleCamera &camera, const std::vector<glm::mat4> &cubeTransforms);

    std::vector<glm::mat4> _queuedExampleCubes;
    jleShader _exampleCubeShader;
    unsigned int _exampleCubeVBO{}, _exampleCubeVAO{}, _exampleCubeInstanceBuffer{};

    jleShader _defaultMeshShader;
    jleShader _pickingShader;
    jleShader _shadowMappingShader;
    jleShader _debugDepthQuad;

    void renderMeshes(const jleCamera &camera, const std::vector<jle3DRendererQueuedMesh> &meshes);

    void renderShadowMeshes(const std::vector<jle3DRendererQueuedMesh> &meshes);

    std::vector<jle3DRendererQueuedMesh> _queuedMeshes;

    void renderSkybox(const jleCamera &camera);
    jleShader _skyboxShader;
    std::shared_ptr<jleSkybox> _skybox;

    std::vector<jle3DRendererLight> _queuedLights;

    void renderDirectionalLight(const jleCamera &camera);
    glm::mat4 _lightSpaceMatrix{};
    float near_plane = -700.0f, far_plane = 700.f;

    std::unique_ptr<jleFramebuffer> _shadowMappingFramebuffer{};
};
