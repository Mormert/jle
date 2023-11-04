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
#include "jleSkinnedMesh.h"
#include "jleAnimationFinalMatrices.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include <RmlUi/Core/Context.h>
#include <RmlUi_Backend.h>

#include <random>

#include <WickedEngine/wiJobSystem.h>

#define JLE_LINE_DRAW_BATCH_SIZE 32768

struct jle3DRenderer::jle3DRendererShaders {
    jle3DRendererShaders()
        : defaultMeshShader{jlePath{"ER:/shaders/defaultMesh.glsl"}},
          missingMaterialShader{jlePath{"ER:/shaders/missingMaterialShader.glsl"}},
          skyboxShader{jlePath{"ER:/shaders/skybox.glsl"}}, pickingShader{jlePath{"ER:/shaders/picking.glsl"}},
          shadowMappingShader{jlePath{"ER:/shaders/shadowMapping.glsl"}},
          shadowMappingPointShader{jlePath{"ER:/shaders/shadowMappingPoint.glsl"}},
          debugDepthQuad{jlePath{"ER:/shaders/debugDepthQuad.glsl"}}, linesShader{jlePath{"ER:/shaders/lines.glsl"}}
    {
    }

    jleResourceRef<jleShader> defaultMeshShader;
    jleResourceRef<jleShader> missingMaterialShader;
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
}

jle3DRenderer::~jle3DRenderer()
{
    glDeleteBuffers(1, &_lineVBO);
    glDeleteVertexArrays(1, &_lineVAO);
}

void
jle3DRenderer::render(jleFramebufferInterface &framebufferOut,
                      const jleCamera &camera,
                      jle3DGraph &graph,
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

    // Sort translucency early on another thread, sync before rendering translucency
    wi::jobsystem::context sortCtx;
    if(!graph._translucentMeshes.empty())
    {
        wi::jobsystem::Execute(sortCtx, [&](wi::jobsystem::JobArgs args){
            sortTranslucentMeshes(camera, graph._translucentMeshes);
        });
    }


    // Directional light renders to the shadow mapping framebuffer
    renderDirectionalLight(camera, graph._meshes, graph._skinnedMeshes, settings);

    glCheckError("3D Render - Directional Lights");

    renderPointLights(camera, graph);

    glCheckError("3D Render - Point Lights");

    framebufferOut.bind();
    glViewport(0, 0, viewportWidth, viewportHeight);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    // glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    bindShadowmapFramebuffers(settings);

    {
        JLE_SCOPE_PROFILE_CPU(jle3DRenderer_renderMeshes_Opaque)
        renderMeshes(camera, graph._meshes, graph._lights, settings);
        glCheckError("3D Render - Opaque Meshes");
    }

    {
        JLE_SCOPE_PROFILE_CPU(jle3DRenderer_renderSkinnedMeshes_Opaque)
        renderSkinnedMeshes(camera, graph._skinnedMeshes, graph._lights, settings);
        glCheckError("3D Render - Opaque Skinned Meshes");
    }

    renderLineStrips(camera, graph._lineStrips);
    glCheckError("3D Render - Strip Lines");

    renderLines(camera, graph._lines);
    glCheckError("3D Render - Lines");

    renderSkybox(camera, settings);
    glCheckError("3D Render - Skybox");

    {
        JLE_SCOPE_PROFILE_CPU(jle3DRenderer_renderMeshes_Translucent)
        if(!graph._translucentMeshes.empty()){
            wi::jobsystem::Wait(sortCtx);
        }

        renderMeshes(camera, graph._translucentMeshes, graph._lights, settings);
        glCheckError("3D Render - Translucent Meshes");
    }

    // gEngine->context->Update();

    /*
     * Temporarily disables RmlUi rendering, causing some problems since it's a WIP
        gEngine->context->SetDimensions(Rml::Vector2i(framebufferOut.width(), framebufferOut.height()));

        // Disable depth testing here because the UI doesnt use depth
        glDisable(GL_DEPTH_TEST);
        Backend::BeginFrame();
        gEngine->context->Render();
        Backend::PresentFrame();

     */

    framebufferOut.bindDefault();
}

void
jle3DRenderer::renderMeshes(const jleCamera &camera,
                            const std::vector<jle3DQueuedMesh> &meshes,
                            const std::vector<jle3DRendererLight> &lights,
                            const jle3DSettings &settings)
{
    for (auto &&mesh : meshes) {
        JLE_SCOPE_PROFILE_GPU(MeshRender);
        if (!mesh.material || !mesh.material->getShader()) {
            _shaders->missingMaterialShader->use();
            _shaders->missingMaterialShader->SetMat4("uView", camera.getViewMatrix());
            _shaders->missingMaterialShader->SetMat4("uProj", camera.getProjectionMatrix());
            _shaders->missingMaterialShader->SetMat4("uModel", mesh.transform);
        } else {
            mesh.material->useMaterial(camera, lights, settings);
            mesh.material->getShader()->SetMat4("uModel", mesh.transform);
            mesh.material->getShader()->SetBool("uUseSkinning", false);
        }

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
jle3DRenderer::renderSkinnedMeshes(const jleCamera &camera,
                                   const std::vector<jle3DQueuedSkinnedMesh> &skinnedMeshes,
                                   const std::vector<jle3DRendererLight> &lights,
                                   const jle3DSettings &settings)
{
    for (auto &&mesh : skinnedMeshes) {
        JLE_SCOPE_PROFILE_GPU(SkinnedMeshRender);
        if (!mesh.material || !mesh.material->getShader()) {
            _shaders->missingMaterialShader->use();
            _shaders->missingMaterialShader->SetMat4("uView", camera.getViewMatrix());
            _shaders->missingMaterialShader->SetMat4("uProj", camera.getProjectionMatrix());
            _shaders->missingMaterialShader->SetMat4("uModel", mesh.transform);
        } else {
            mesh.material->useMaterial(camera, lights, settings);
            mesh.material->getShader()->SetMat4("uModel", mesh.transform);
            mesh.material->getShader()->SetBool("uUseSkinning", true);

            for(int i = 0; i < mesh.matrices->matrices.size(); ++i){
                mesh.material->getShader()->SetMat4("uAnimBonesMatrices[" + std::to_string(i) + "]", mesh.matrices->matrices[i]);
            }
        }

        glBindVertexArray(mesh.skinnedMesh->getVAO());
        if (mesh.skinnedMesh->usesIndexing()) {
            glDrawElements(GL_TRIANGLES, mesh.skinnedMesh->getTrianglesCount(), GL_UNSIGNED_INT, (void *)0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, mesh.skinnedMesh->getTrianglesCount());
        }
        glBindVertexArray(0);
    }
}

void
jle3DRenderer::sortTranslucentMeshes(const jleCamera &camera, std::vector<jle3DQueuedMesh> &translucentMeshes)
{
    ZoneScoped;
    glm::vec3 camPosition = camera.getPosition();

    std::sort(translucentMeshes.begin(),
              translucentMeshes.end(),
              [&camPosition](const jle3DQueuedMesh &mesh1, const jle3DQueuedMesh &mesh2) {
                  return glm::distance2(camPosition, glm::vec3(mesh1.transform[3])) >
                         glm::distance2(camPosition, glm::vec3(mesh2.transform[3]));
              });
}

void
jle3DRenderer::renderSkybox(const jleCamera &camera, const jle3DSettings &settings)
{
    JLE_SCOPE_PROFILE_CPU(jle3DRenderer_renderSkybox)

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

    const auto executeRender = [&](const std::vector<jle3DQueuedMesh> &meshes) {
        for (auto &&mesh : meshes) {
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
    };

    executeRender(graph._meshes);
    executeRender(graph._translucentMeshes);

    framebufferOut.bindDefault();
}

void
jle3DRenderer::renderDirectionalLight(const jleCamera &camera,
                                      const std::vector<jle3DQueuedMesh> &meshes,
                                      const std::vector<jle3DQueuedSkinnedMesh> &skinnedMeshes,
                                      const jle3DSettings &settings)
{
    JLE_SCOPE_PROFILE_CPU(jle3DRenderer_renderDirectionalLight)

    if (!settings.useDirectionalLight) {
        return;
    }

    _shadowMappingFramebuffer->bind();

    glDisable(GL_CULL_FACE);

    _shaders->shadowMappingShader->use();

    _shaders->shadowMappingShader->SetMat4("lightSpaceMatrix", settings.getLightSpaceMatrixAtPosition(camera.getPosition()));

    glViewport(0, 0, (int)_shadowMappingFramebuffer->width(), (int)_shadowMappingFramebuffer->height());

    glClear(GL_DEPTH_BUFFER_BIT);
    renderShadowMeshes(meshes, *_shaders->shadowMappingShader.get());
    renderShadowMeshesSkinned(skinnedMeshes, *_shaders->shadowMappingShader.get());

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
        renderShadowMeshesSkinned(graph._skinnedMeshes, *_shaders->shadowMappingPointShader.get());
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
        if (mesh.material) {
            if (auto opacity = mesh.material->getOpacityTexture()) {
                shader.SetBool("uUseOpacityTexture", true);
                shader.SetTextureSlot("uOpacityTexture", jleTextureSlot::Opacity);
                opacity->setActive(jleTextureSlot::Opacity);
            } else {
                shader.SetBool("uUseOpacityTexture", false);
            }
        } else {
            shader.SetBool("uUseOpacityTexture", false);
        }

        shader.SetBool("uUseSkinning", false);

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
jle3DRenderer::renderShadowMeshesSkinned(const std::vector<jle3DQueuedSkinnedMesh> &skinnedMeshes, jleShader &shader)
{
    for (auto &&mesh : skinnedMeshes) {
        if (!mesh.castShadows) {
            return;
        }
        shader.SetMat4("model", mesh.transform);
        if (mesh.material) {
            if (auto opacity = mesh.material->getOpacityTexture()) {
                shader.SetBool("uUseOpacityTexture", true);
                shader.SetTextureSlot("uOpacityTexture", jleTextureSlot::Opacity);
                opacity->setActive(jleTextureSlot::Opacity);
            } else {
                shader.SetBool("uUseOpacityTexture", false);
            }
        } else {
            shader.SetBool("uUseOpacityTexture", false);
        }

        shader.SetBool("uUseSkinning", true);

        for(int i = 0; i < mesh.matrices->matrices.size(); ++i){
            mesh.material->getShader()->SetMat4("uAnimBonesMatrices[" + std::to_string(i) + "]", mesh.matrices->matrices[i]);
        }

        glBindVertexArray(mesh.skinnedMesh->getVAO());
        if (mesh.skinnedMesh->usesIndexing()) {
            glDrawElements(GL_TRIANGLES, mesh.skinnedMesh->getTrianglesCount(), GL_UNSIGNED_INT, (void *)0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, mesh.skinnedMesh->getTrianglesCount());
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

void
jle3DRenderer::bindShadowmapFramebuffers(const jle3DSettings &settings)
{
    // Bind to shadow map texture
    glActiveTexture(JLE_TEXTURE_DIR_SHADOW);
    glBindTexture(GL_TEXTURE_2D, _shadowMappingFramebuffer->texture());

    glActiveTexture(JLE_TEXTURE_POINT_SHADOW);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _pointsShadowMappingFramebuffer->texture());

    if (settings.skybox) {
        glActiveTexture(JLE_TEXTURE_SKYBOX);
        glBindTexture(GL_TEXTURE_CUBE_MAP, settings.skybox->getTextureID());
    }
}
