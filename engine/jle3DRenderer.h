// Copyright (c) 2023. Johan Lind

#pragma once

#include "jle3DGraph.h"
#include "jle3DSettings.h"
#include "jleCamera.h"
#include "jleFrameBufferInterface.h"

#include <memory>
#include <vector>

class jleFramebufferShadowCubeMap;
class jleFramebufferShadowMap;
class jleShader;

class jle3DRenderer
{
public:
    jle3DRenderer();

    virtual ~jle3DRenderer();

    void render(jleFramebufferInterface &framebufferOut,
                const jleCamera &camera,
                jle3DGraph &graph,
                const jle3DSettings &settings);

    void renderMeshesPicking(jleFramebufferInterface &framebufferOut, const jleCamera &camera, const jle3DGraph &graph);

private:
    void renderMeshes(const jleCamera &camera,
                      const std::vector<jle3DQueuedMesh> &meshes,
                      const std::vector<jle3DRendererLight> &lights,
                      const jle3DSettings &settings);

    void renderSkinnedMeshes(const jleCamera &camera,
                             const std::vector<jle3DQueuedSkinnedMesh> &skinnedMeshes,
                             const std::vector<jle3DRendererLight> &lights,
                             const jle3DSettings &settings);

    void sortTranslucentMeshes(const jleCamera &camera, std::vector<jle3DQueuedMesh> &translucentMeshes);

    void renderShadowMeshes(const std::vector<jle3DQueuedMesh> &meshes, jleShader &shader);

    void renderShadowMeshesSkinned(const std::vector<jle3DQueuedSkinnedMesh> &skinnedMeshes, jleShader &shader);

    void renderLines(const jleCamera &camera, const std::vector<jle3DLineVertex> &linesBatch);

    void renderLineStrips(const jleCamera &camera, const std::vector<std::vector<jle3DLineVertex>> &lineStripBatch);

    void renderSkybox(const jleCamera &camera, const jle3DSettings &settings);

    void renderDirectionalLight(const jleCamera &camera,
                                const std::vector<jle3DQueuedMesh> &meshes,
                                const std::vector<jle3DQueuedSkinnedMesh> &skinnedMeshes,
                                const jle3DSettings &settings);

    void renderPointLights(const jleCamera &camera, const jle3DGraph &graph);

    void bindShadowmapFramebuffers(const jle3DSettings &settings);

    struct jle3DRendererShaders;
    std::unique_ptr<jle3DRendererShaders> _shaders;

    std::unique_ptr<jleFramebufferShadowMap> _shadowMappingFramebuffer{};
    std::unique_ptr<jleFramebufferShadowCubeMap> _pointsShadowMappingFramebuffer{};

    unsigned int _lineVAO{}, _lineVBO{};
};
