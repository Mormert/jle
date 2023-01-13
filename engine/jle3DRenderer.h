// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFramebufferShadowCubeMap.h"
#include "jleFramebufferShadowMap.h"
#include "jleSkybox.h"
#include "jleCamera.h"
#include "jleMesh.h"
#include "jleMaterial.h"
#include "jleShader.h"
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

class jle3DRenderer
{
public:
    struct jle3DRendererQueuedMesh {
        glm::mat4 transform;
        std::shared_ptr<jleMesh> mesh;
        std::shared_ptr<jleMaterial> material;
        int instanceId;
    };

    struct jle3DRendererLight {
        glm::vec3 position;
        glm::vec3 color;
    };

    jle3DRenderer();

    virtual ~jle3DRenderer();

    void queuerender(jleFramebufferInterface &framebufferOut, const jleCamera &camera);

    void render(jleFramebufferInterface &framebufferOut,
                const jleCamera &camera,
                const std::vector<glm::mat4> &cubeTransforms,
                const std::vector<jle3DRendererQueuedMesh> &meshes);

    void renderMeshesPicking(jleFramebufferInterface &framebufferOut, const jleCamera &camera);

    void sendExampleCube(const glm::mat4 &transform);

    void sendMesh(const std::shared_ptr<jleMesh> &mesh, const std::shared_ptr<jleMaterial> &material, const glm::mat4 &transform, int instanceId);

    void sendLight(const glm::vec3 &position, const glm::vec3 &color);

    void enableDirectionalLight();

    void disableDirectionalLight();

    void setDirectionalLight(const glm::mat4 &view, const glm::vec3 &colour);

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
    jleShader _shadowMappingPointShader;
    jleShader _debugDepthQuad;

    void renderMeshes(const jleCamera &camera, const std::vector<jle3DRendererQueuedMesh> &meshes);

    void renderShadowMeshes(const std::vector<jle3DRendererQueuedMesh> &meshes, jleShader& shader);

    std::vector<jle3DRendererQueuedMesh> _queuedMeshes;

    void renderSkybox(const jleCamera &camera);
    jleShader _skyboxShader;
    std::shared_ptr<jleSkybox> _skybox;

    std::vector<jle3DRendererLight> _queuedLights;

    void renderDirectionalLight(const jleCamera &camera);
    glm::mat4 _lightSpaceMatrix{};
    float near_plane = -700.0f, far_plane = 700.f;

    void renderPointLights(const jleCamera& camera);

    std::unique_ptr<jleFramebufferShadowMap> _shadowMappingFramebuffer{};
    std::unique_ptr<jleFramebufferShadowCubeMap> _pointsShadowMappingFramebuffer{};

    bool _useDirectionalLight{false};
    bool _useEnvironmentMapping{true};
    glm::vec3 _directionalLightRotation{};
    glm::vec3 _directionalLightColour{};
};
