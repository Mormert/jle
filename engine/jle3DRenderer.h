// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleCamera.h"
#include "jleFramebufferShadowCubeMap.h"
#include "jleFramebufferShadowMap.h"
#include "jleMesh.h"
#include "jleResourceRef.h"
#include "jleShader.h"
#include "jleSkybox.h"
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

class jleMaterial;

class jle3DRenderer
{
public:
    struct jle3DRendererQueuedMesh {
        glm::mat4 transform;
        std::shared_ptr<jleMesh> mesh;
        std::shared_ptr<jleMaterial> material;
        int instanceId;
        bool castShadows;
    };

    struct jle3DRendererLight {
        glm::vec3 position;
        glm::vec3 color;
    };

    struct jle3DRendererLines {
        std::vector<glm::vec3> points;
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

    void sendMesh(std::shared_ptr<jleMesh> &mesh,
                  std::shared_ptr<jleMaterial> &material,
                  const glm::mat4 &transform,
                  int instanceId,
                  bool castShadows);

    void sendLines(const std::vector<glm::vec3>& points, const glm::vec3& colour);

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
    unsigned int _exampleCubeVBO{}, _exampleCubeVAO{}, _exampleCubeInstanceBuffer{};

    /*
    jleShader _exampleCubeShader;
    jleShader _defaultMeshShader;
    jleShader _pickingShader;
    jleShader _shadowMappingShader;
    jleShader _shadowMappingPointShader;
    jleShader _debugDepthQuad;*/

    jleResourceRef<jleShader> _exampleCubeShader;
    jleResourceRef<jleShader> _defaultMeshShader;
    jleResourceRef<jleShader> _pickingShader;
    jleResourceRef<jleShader> _shadowMappingShader;
    jleResourceRef<jleShader> _shadowMappingPointShader;
    jleResourceRef<jleShader> _debugDepthQuad;
    jleResourceRef<jleShader> _linesShader;

    void renderMeshes(const jleCamera &camera, const std::vector<jle3DRendererQueuedMesh> &meshes);

    void renderShadowMeshes(const std::vector<jle3DRendererQueuedMesh> &meshes, jleShader &shader);

    void renderLines(const jleCamera &camera, const std::vector<jle3DRendererLines>& linesBatch);

    std::vector<jle3DRendererQueuedMesh> _queuedMeshes;

    std::vector<jle3DRendererLines> _queuedLines;

    void renderSkybox(const jleCamera &camera);
    jleResourceRef<jleShader> _skyboxShader;
    std::shared_ptr<jleSkybox> _skybox;

    std::vector<jle3DRendererLight> _queuedLights;

    void renderDirectionalLight(const jleCamera &camera);
    glm::mat4 _lightSpaceMatrix{};
    float near_plane = -700.0f, far_plane = 700.f;

    void renderPointLights(const jleCamera &camera);

    std::unique_ptr<jleFramebufferShadowMap> _shadowMappingFramebuffer{};
    std::unique_ptr<jleFramebufferShadowCubeMap> _pointsShadowMappingFramebuffer{};

    bool _useDirectionalLight{false};
    bool _useEnvironmentMapping{true};
    glm::vec3 _directionalLightRotation{};
    glm::vec3 _directionalLightColour{};

    unsigned int _lineVAO, _lineVBO;
};
