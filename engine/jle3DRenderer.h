// Copyright (c) 2023. Johan Lind

#pragma once

#include "jle3DRendererGraph.h"
#include "jle3DRendererSettings.h"
#include "jleCamera.h"
#include "jleMesh.h"
#include "jleResourceRef.h"
#include "jleShader.h"
#include "jleSkybox.h"

#define JLE_LINE_DRAW_BATCH_SIZE 32768

class jleFramebufferShadowCubeMap;
class jleFramebufferShadowMap;

class jle3DRenderer
{
public:
    jle3DRenderer();

    virtual ~jle3DRenderer();

    void render(jleFramebufferInterface &framebufferOut,
                const jleCamera &camera,
                const jle3DRendererGraph &graph,
                const jle3DRendererSettings &settings);

    void renderMeshesPicking(jleFramebufferInterface &framebufferOut, const jleCamera &camera, const jle3DRendererGraph &graph);

private:

    void renderMeshes(const jleCamera &camera, const jle3DRendererGraph& graph, const jle3DRendererSettings& settings);

    void renderShadowMeshes(const std::vector<jle3DRendererGraph::jle3DRendererQueuedMesh> &meshes, jleShader &shader);

    void renderLines(const jleCamera &camera, const std::vector<jle3DLineVertex> &linesBatch);

    void renderLineStrips(const jleCamera &camera, const std::vector<std::vector<jle3DLineVertex>> &lineStripBatch);

    void renderSkybox(const jleCamera &camera, const jle3DRendererSettings& settings);

    void renderDirectionalLight(const std::vector<jle3DRendererGraph::jle3DRendererQueuedMesh> &meshes, const jle3DRendererSettings& settings);

    void renderPointLights(const jleCamera &camera, const jle3DRendererGraph &graph);

    jleResourceRef<jleShader> _exampleCubeShader;
    jleResourceRef<jleShader> _defaultMeshShader;
    jleResourceRef<jleShader> _pickingShader;
    jleResourceRef<jleShader> _shadowMappingShader;
    jleResourceRef<jleShader> _shadowMappingPointShader;
    jleResourceRef<jleShader> _debugDepthQuad;
    jleResourceRef<jleShader> _linesShader;
    jleResourceRef<jleShader> _skyboxShader;

    std::unique_ptr<jleFramebufferShadowMap> _shadowMappingFramebuffer{};
    std::unique_ptr<jleFramebufferShadowCubeMap> _pointsShadowMappingFramebuffer{};

    unsigned int _lineVAO, _lineVBO;
};
