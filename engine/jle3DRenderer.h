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

#define JLE_LINE_DRAW_BATCH_SIZE 32768

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

    struct jle3DLineVertex
    {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec3 attenuation; // {1.f, 0.f, 0.f} means no attenuation (constant)
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

    // Line strips will always connect each lines, from start to end.
    void sendLineStrip(const std::vector<jle3DLineVertex> &lines);

    // Lines needs to come in pairs in the points, two for each line.
    void sendLines(const std::vector<jle3DLineVertex> &lines);

    void sendLine(const jle3DLineVertex& from, const jle3DLineVertex& to);

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

    jleResourceRef<jleShader> _exampleCubeShader;
    jleResourceRef<jleShader> _defaultMeshShader;
    jleResourceRef<jleShader> _pickingShader;
    jleResourceRef<jleShader> _shadowMappingShader;
    jleResourceRef<jleShader> _shadowMappingPointShader;
    jleResourceRef<jleShader> _debugDepthQuad;
    jleResourceRef<jleShader> _linesShader;

    void renderMeshes(const jleCamera &camera, const std::vector<jle3DRendererQueuedMesh> &meshes);

    void renderShadowMeshes(const std::vector<jle3DRendererQueuedMesh> &meshes, jleShader &shader);

    void renderLines(const jleCamera &camera, const std::vector<jle3DLineVertex>& linesBatch);

    void renderLineStrips(const jleCamera &camera, std::vector<std::vector<jle3DLineVertex>>& lineStripBatch);

    std::vector<jle3DRendererQueuedMesh> _queuedMeshes;

    std::vector<std::vector<jle3DLineVertex>> _queuedLineStrips;
    std::vector<jle3DLineVertex> _queuedLines;

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
