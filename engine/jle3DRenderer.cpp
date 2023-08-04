// Copyright (c) 2023. Johan Lind

#include "jle3DRenderer.h"
#include "jle3DGraph.h"
#include "jle3DSettings.h"
#include "jleCamera.h"
#include "jleFrameBufferInterface.h"
#include "jleFramebufferShadowCubeMap.h"
#include "jleFramebufferShadowMap.h"
#include "jleFullscreenRendering.h"
#include "jleGLError.h"
#include "jleGameEngine.h"
#include "jleIncludeGL.h"
#include "jleMaterial.h"
#include "jleMesh.h"
#include "jleProfiler.h"
#include "jleShader.h"
#include "jleSkybox.h"
#include "jleStaticOpenGLState.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <RmlUi/Core/Context.h>
#include <RmlUi_Backend.h>

#include <random>

#define JLE_LINE_DRAW_BATCH_SIZE 32768

struct jle3DRenderer::jle3DRendererShaders {
    jle3DRendererShaders()
        : exampleCubeShader{jlePath{"ER:/shaders/exampleCube.glsl"}},
          defaultMeshShader{jlePath{"ER:/shaders/defaultMesh.glsl"}}, skyboxShader{jlePath{"ER:/shaders/skybox.glsl"}},
          pickingShader{jlePath{"ER:/shaders/picking.glsl"}},
          shadowMappingShader{jlePath{"ER:/shaders/shadowMapping.glsl"}},
          shadowMappingPointShader{jlePath{"ER:/shaders/shadowMappingPoint.glsl"}},
          debugDepthQuad{jlePath{"ER:/shaders/depthDebug.glsl"}}, linesShader{jlePath{"ER:/shaders/lines.glsl"}}
    {
    }

    jleResourceRef<jleShader> exampleCubeShader;
    jleResourceRef<jleShader> defaultMeshShader;
    jleResourceRef<jleShader> pickingShader;
    jleResourceRef<jleShader> shadowMappingShader;
    jleResourceRef<jleShader> shadowMappingPointShader;
    jleResourceRef<jleShader> debugDepthQuad;
    jleResourceRef<jleShader> linesShader;
    jleResourceRef<jleShader> skyboxShader;
};

jle3DRenderer::jle3DRenderer()
{
    _shaders = std::make_unique<jle3DRendererShaders>();

    // Generate buffers for line drawing
    glGenVertexArrays(1, &_lineVAO);
    glGenBuffers(1, &_lineVBO);
    glBindVertexArray(_lineVAO);

    glBindBuffer(GL_ARRAY_BUFFER, _lineVBO);
    glBufferData(
        GL_ARRAY_BUFFER, (GLuint)JLE_LINE_DRAW_BATCH_SIZE * sizeof(jle3DLineVertex), (void *)0, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(jle3DLineVertex), (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(jle3DLineVertex), (void *)(1 * sizeof(glm::vec3)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(jle3DLineVertex), (void *)(2 * sizeof(glm::vec3)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // End gen buffers for line drawing

    _shadowMappingFramebuffer = std::make_unique<jleFramebufferShadowMap>(2048, 2048);
    _pointsShadowMappingFramebuffer = std::make_unique<jleFramebufferShadowCubeMap>(1024, 1024);

    /*
    glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
    jleCameraSimpleFPVController jc;
    glm::mat4 lightView =
        glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), glm::vec3(2.0f, 3.0f, 2.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    lightView[1][1] *= -1.f;*/
}

jle3DRenderer::~jle3DRenderer()
{
    glDeleteBuffers(1, &_lineVBO);
    glDeleteVertexArrays(1, &_lineVAO);
}

void
jle3DRenderer::render(jleFramebufferInterface &framebufferOut,
                      const jleCamera &camera,
                      const jle3DGraph &graph,
                      const jle3DSettings &settings)
{
    JLE_SCOPE_PROFILE_CPU(jle3DRenderer_render)

    framebufferOut.bind();

    const auto backgroundColor = camera.getBackgroundColor();
    glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const int viewportWidth = framebufferOut.width();
    const int viewportHeight = framebufferOut.height();

    glEnable(GL_DEPTH_TEST);

    // Directional light renders to the shadow mapping framebuffer
    renderDirectionalLight(graph._meshes, settings);

    glCheckError("3D Render - Directional Lights");

    renderPointLights(camera, graph);

    glCheckError("3D Render - Point Lights");

    framebufferOut.bind();
    glViewport(0, 0, viewportWidth, viewportHeight);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    // glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    renderMeshes(camera, graph, settings);

    glCheckError("3D Render - Meshes");

    renderLineStrips(camera, graph._lineStrips);

    glCheckError("3D Render - Strip Lines");

    renderLines(camera, graph._lines);

    glCheckError("3D Render - Lines");

    renderSkybox(camera, settings);

    glCheckError("3D Render - Skybox");

    // gEngine->context->Update();

    gEngine->context->SetDimensions(Rml::Vector2i(framebufferOut.width(), framebufferOut.height()));

    // Disable depth testing here because the UI doesnt use depth
    glDisable(GL_DEPTH_TEST);
    Backend::BeginFrame();
    gEngine->context->Render();
    Backend::PresentFrame();

    framebufferOut.bindDefault();
}

void
jle3DRenderer::renderMeshes(const jleCamera &camera, const jle3DGraph &graph, const jle3DSettings &settings)
{
    JLE_SCOPE_PROFILE_CPU(jle3DRenderer_renderMeshes)

    if (graph._meshes.empty()) {
        return;
    }

    // Bind to shadow map texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _shadowMappingFramebuffer->texture());
    jleStaticOpenGLState::globalActiveTexture = _shadowMappingFramebuffer->texture();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _pointsShadowMappingFramebuffer->texture());

    if (settings.skybox) {
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, settings.skybox->getTextureID());
    }

    _shaders->defaultMeshShader->use();
    _shaders->defaultMeshShader->SetInt("shadowMap", 0);
    _shaders->defaultMeshShader->SetInt("shadowMapPoint", 1);
    _shaders->defaultMeshShader->SetInt("albedoTexture", 2);
    _shaders->defaultMeshShader->SetInt("normalTexture", 3);
    _shaders->defaultMeshShader->SetInt("skyboxTexture", 4);
    _shaders->defaultMeshShader->SetFloat("farPlane", 500.f);
    _shaders->defaultMeshShader->SetBool("UseDirectionalLight", settings.useDirectionalLight);
    _shaders->defaultMeshShader->SetBool("UseEnvironmentMapping", settings.useEnvironmentMapping);
    _shaders->defaultMeshShader->SetVec3("DirectionalLightColour", settings.directionalLightColour);
    _shaders->defaultMeshShader->SetVec3("DirectionalLightDir", settings.directionalLightRotation);
    _shaders->defaultMeshShader->SetMat4("view", camera.getViewMatrix());
    _shaders->defaultMeshShader->SetMat4("proj", camera.getProjectionMatrix());
    _shaders->defaultMeshShader->SetMat4("lightSpaceMatrix", settings.lightSpaceMatrix);
    _shaders->defaultMeshShader->SetVec3("CameraPosition", camera.getPosition());
    _shaders->defaultMeshShader->SetInt("LightsCount", (int)graph._lights.size());

    // Limit to 4 lights
    int lightCount = graph._lights.size();
    if (lightCount > 4) {
        lightCount = 4;
    }

    for (int l = 0; l < lightCount; l++) {
        _shaders->defaultMeshShader->SetVec3("LightPositions[" + std::to_string(l) + "]", graph._lights[l].position);
        _shaders->defaultMeshShader->SetVec3("LightColors[" + std::to_string(l) + "]", graph._lights[l].color);
    }

    for (auto &&mesh : graph._meshes) {
        _shaders->defaultMeshShader->SetMat4("model", mesh.transform);

        // Set textures
        if (mesh.material) {
            if (mesh.material->_albedoTextureRef) {
                mesh.material->_albedoTextureRef.get()->setActive(2);
                _shaders->defaultMeshShader->SetBool("useAlbedoTexture", true);
            } else {
                _shaders->defaultMeshShader->SetBool("useAlbedoTexture", false);
            }
            if (mesh.material->_normalTextureRef) {
                mesh.material->_normalTextureRef.get()->setActive(3);
                _shaders->defaultMeshShader->SetBool("useNormalTexture", true);
            } else {
                _shaders->defaultMeshShader->SetBool("useNormalTexture", false);
            }
        } else {
            _shaders->defaultMeshShader->SetBool("useAlbedoTexture", false);
            _shaders->defaultMeshShader->SetBool("useNormalTexture", false);
        }

        glBindVertexArray(mesh.mesh->getVAO());
        if (mesh.mesh->usesIndexing()) {
            glDrawElements(GL_TRIANGLES, mesh.mesh->getTrianglesCount(), GL_UNSIGNED_INT, (void *)0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, mesh.mesh->getTrianglesCount());
        }
        glBindVertexArray(0);

        // Unset textures
        if (mesh.material) {
            if (mesh.material->_albedoTextureRef) {
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            if (mesh.material->_normalTextureRef) {
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }
    }

    glActiveTexture(GL_TEXTURE0);
}

void
jle3DRenderer::renderSkybox(const jleCamera &camera, const jle3DSettings &settings)
{
    JLE_SCOPE_PROFILE_CPU(jle3DRenderer_renderMeshes)

    if (!settings.skybox) {
        return;
    }

    if (camera.getProjectionType() == jleCameraProjection::Orthographic) {
        return;
    }

    // Depth testing to draw the skybox behind everything.
    // We also draw the skybox last, such that it can be early-depth-tested.
    glDepthFunc(GL_LEQUAL);
    _shaders->skyboxShader->use();

    // Convert the view matrix to mat3 first to remove the translation
    auto view = glm::mat4(glm::mat3(camera.getViewMatrix()));

    _shaders->skyboxShader->SetMat4("view", view);
    _shaders->skyboxShader->SetMat4("projection", camera.getProjectionMatrix());
    // skybox cube
    glBindVertexArray(settings.skybox->getVAO());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, settings.skybox->getTextureID());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
}

void
jle3DRenderer::renderMeshesPicking(jleFramebufferInterface &framebufferOut,
                                   const jleCamera &camera,
                                   const jle3DGraph &graph)
{
    JLE_SCOPE_PROFILE_CPU(jle3DRenderer_renderMeshesPicking)

    const int viewportWidth = framebufferOut.width();
    const int viewportHeight = framebufferOut.height();

    framebufferOut.bind();

    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Change viewport dimensions to match framebuffer's dimensions
    glViewport(0, 0, viewportWidth, viewportHeight);

    _shaders->pickingShader->use();
    _shaders->pickingShader->SetMat4("projView", camera.getProjectionViewMatrix());

    for (auto &&mesh : graph._meshes) {
        int r = (mesh.instanceId & 0x000000FF) >> 0;
        int g = (mesh.instanceId & 0x0000FF00) >> 8;
        int b = (mesh.instanceId & 0x00FF0000) >> 16;
        _shaders->pickingShader->SetVec4("PickingColor", glm::vec4{r / 255.0f, g / 255.0f, b / 255.0f, 1.f});
        _shaders->pickingShader->SetMat4("model", mesh.transform);
        glBindVertexArray(mesh.mesh->getVAO());
        if (mesh.mesh->usesIndexing()) {
            glDrawElements(GL_TRIANGLES, mesh.mesh->getTrianglesCount(), GL_UNSIGNED_INT, (void *)0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, mesh.mesh->getTrianglesCount());
        }
        glBindVertexArray(0);
    }

    framebufferOut.bindDefault();
}

void
jle3DRenderer::renderDirectionalLight(const std::vector<jle3DQueuedMesh> &meshes, const jle3DSettings &settings)
{
    JLE_SCOPE_PROFILE_CPU(jle3DRenderer_renderDirectionalLight)

    if (!settings.useDirectionalLight) {
        return;
    }

    _shadowMappingFramebuffer->bind();

    glDisable(GL_CULL_FACE);

    _shaders->shadowMappingShader->use();

    _shaders->shadowMappingShader->SetMat4("lightSpaceMatrix", settings.lightSpaceMatrix);

    glViewport(0, 0, (int)_shadowMappingFramebuffer->width(), (int)_shadowMappingFramebuffer->height());

    glClear(GL_DEPTH_BUFFER_BIT);
    renderShadowMeshes(meshes, *_shaders->shadowMappingShader.get());

    glEnable(GL_CULL_FACE);

    _shadowMappingFramebuffer->bindDefault();
}

void
jle3DRenderer::renderPointLights(const jleCamera &camera, const jle3DGraph &graph)
{
    JLE_SCOPE_PROFILE_CPU(jle3DRenderer_renderPointLights)

    if (graph._lights.empty()) {
        return;
    }

    glCullFace(GL_BACK);

    float aspect = (float)_pointsShadowMappingFramebuffer->width() / (float)_pointsShadowMappingFramebuffer->height();
    float nearP = 0.0f;
    float farP = 500.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, nearP, farP);

    glm::vec3 lightPos = graph._lights[0].position;

    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(shadowProj *
                               glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj *
                               glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj *
                               glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    shadowTransforms.push_back(shadowProj *
                               glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
    shadowTransforms.push_back(shadowProj *
                               glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj *
                               glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

    _shaders->shadowMappingPointShader->use();
    _shaders->shadowMappingPointShader->SetVec3("lightPos", lightPos);
    _shaders->shadowMappingPointShader->SetFloat("farPlane", farP);

    for (int i = 0; i < 6; i++) {
        _shaders->shadowMappingPointShader->SetMat4("lightSpaceMatrix", shadowTransforms[i]);
        _pointsShadowMappingFramebuffer->setRenderFace(i);
        glViewport(0, 0, (int)_pointsShadowMappingFramebuffer->width(), (int)_pointsShadowMappingFramebuffer->height());

        glClear(GL_DEPTH_BUFFER_BIT);
        renderShadowMeshes(graph._meshes, *_shaders->shadowMappingPointShader.get());
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void
jle3DRenderer::renderShadowMeshes(const std::vector<jle3DQueuedMesh> &meshes, jleShader &shader)
{
    for (auto &&mesh : meshes) {
        if (!mesh.castShadows) {
            return;
        }
        shader.SetMat4("model", mesh.transform);
        glBindVertexArray(mesh.mesh->getVAO());
        if (mesh.mesh->usesIndexing()) {
            glDrawElements(GL_TRIANGLES, mesh.mesh->getTrianglesCount(), GL_UNSIGNED_INT, (void *)0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, mesh.mesh->getTrianglesCount());
        }
        glBindVertexArray(0);
    }
}

void
jle3DRenderer::renderLines(const jleCamera &camera, const std::vector<jle3DLineVertex> &linesBatch)
{
    JLE_SCOPE_PROFILE_CPU(jle3DRenderer_renderLines)

    if (linesBatch.empty()) {
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    _shaders->linesShader->use();
    _shaders->linesShader->SetMat4("projView", camera.getProjectionViewMatrix());
    _shaders->linesShader->SetVec3("cameraPos", camera.getPosition());

    int linesRendered = 0;

    do {
        int batchSize = JLE_LINE_DRAW_BATCH_SIZE;
        if (linesRendered + batchSize > linesBatch.size()) {
            batchSize = linesBatch.size() - linesRendered;
        }

        glBindVertexArray(_lineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, _lineVBO);

        glBufferSubData(GL_ARRAY_BUFFER, 0, batchSize * sizeof(jle3DLineVertex), &linesBatch[linesRendered]);
        glDrawArrays(GL_LINES, 0, batchSize);

        linesRendered += JLE_LINE_DRAW_BATCH_SIZE;
    } while (linesRendered < linesBatch.size());

    glBindVertexArray(0);

    glDisable(GL_BLEND);
}

void
jle3DRenderer::renderLineStrips(const jleCamera &camera,
                                const std::vector<std::vector<jle3DLineVertex>> &lineStripBatch)
{
    JLE_SCOPE_PROFILE_CPU(jle3DRenderer_renderLineStrips)

    // Not implemented
}