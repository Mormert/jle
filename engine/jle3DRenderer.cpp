// Copyright (c) 2022. Johan Lind

#include "jle3DRenderer.h"

#include "jleCamera.h"
#include "jleFrameBufferInterface.h"
#include "jleFullscreenRendering.h"
#include "jleGameEngine.h"
#include "jlePathDefines.h"
#include "jleProfiler.h"
#include "jleStaticOpenGLState.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "jleIncludeGL.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <random>

jle3DRenderer::jle3DRenderer()
    : _exampleCubeShader{std::string{JLE_ENGINE_PATH_SHADERS + "/exampleCube.vert"}.c_str(),
                         std::string{JLE_ENGINE_PATH_SHADERS + "/exampleCube.frag"}.c_str()},
      _defaultMeshShader{std::string{JLE_ENGINE_PATH_SHADERS + "/defaultMesh.vert"}.c_str(),
                         std::string{JLE_ENGINE_PATH_SHADERS + "/defaultMesh.frag"}.c_str()},
      _skyboxShader{std::string{JLE_ENGINE_PATH_SHADERS + "/skybox.vert"}.c_str(),
                    std::string{JLE_ENGINE_PATH_SHADERS + "/skybox.frag"}.c_str()},
      _pickingShader{std::string{JLE_ENGINE_PATH_SHADERS + "/picking.vert"}.c_str(),
                     std::string{JLE_ENGINE_PATH_SHADERS + "/picking.frag"}.c_str()},
      _shadowMappingShader{std::string{JLE_ENGINE_PATH_SHADERS + "/shadowMapping.vert"}.c_str(),
                           std::string{JLE_ENGINE_PATH_SHADERS + "/shadowMapping.frag"}.c_str()},
      _shadowMappingPointShader{std::string{JLE_ENGINE_PATH_SHADERS + "/shadowMappingPoint.vert"}.c_str(),
                                std::string{JLE_ENGINE_PATH_SHADERS + "/shadowMappingPoint.frag"}.c_str()},
      _debugDepthQuad{std::string{JLE_ENGINE_PATH_SHADERS + "/debugDepthQuad.vert"}.c_str(),
                      std::string{JLE_ENGINE_PATH_SHADERS + "/debugDepthQuad.frag"}.c_str()}
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

    _shadowMappingFramebuffer = std::make_unique<jleFramebufferShadowMap>(2048, 2048);
    _pointsShadowMappingFramebuffer = std::make_unique<jleFramebufferShadowCubeMap>(1024, 1024);

    glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
    jleCameraSimpleFPVController jc;
    glm::mat4 lightView =
        glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), glm::vec3(2.0f, 3.0f, 2.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    lightView[1][1] *= -1.f;

    _lightSpaceMatrix = lightProjection * lightView;
}

jle3DRenderer::~jle3DRenderer()
{
    glDeleteBuffers(1, &_exampleCubeVBO);
    glDeleteBuffers(1, &_exampleCubeInstanceBuffer);
    glDeleteVertexArrays(1, &_exampleCubeVAO);
}

void
jle3DRenderer::queuerender(jleFramebufferInterface &framebufferOut, const jleCamera &camera)
{
    render(framebufferOut, camera, _queuedExampleCubes, _queuedMeshes);
}

void
jle3DRenderer::render(jleFramebufferInterface &framebufferOut,
                      const jleCamera &camera,
                      const std::vector<glm::mat4> &cubeTransforms,
                      const std::vector<jle3DRendererQueuedMesh> &meshes)
{
    JLE_SCOPE_PROFILE(jle3DRenderer::render)

    const int viewportWidth = framebufferOut.width();
    const int viewportHeight = framebufferOut.height();

    glEnable(GL_DEPTH_TEST);

    // Directional light renders to the shadow mapping framebuffer
    renderDirectionalLight(camera);

    renderPointLights(camera);

    framebufferOut.bind();

    // Change viewport dimensions to match framebuffer's dimensions
    glViewport(0, 0, viewportWidth, viewportHeight);

    renderExampleCubes(camera, cubeTransforms);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    renderMeshes(camera, _queuedMeshes);

    // renderSkybox(camera);

    /* // Render shadow map in fullscreen as debug
    _debugDepthQuad.use();
    _debugDepthQuad.use();
    _debugDepthQuad.SetInt("depthMap", 0);
    _debugDepthQuad.SetFloat("near_plane", near_plane);
    _debugDepthQuad.SetFloat("far_plane", far_plane);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _shadowMappingFramebuffer->texture());
    renderFullscreenQuad(); */

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
    _queuedMeshes.clear();
    _queuedLights.clear();
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

void
jle3DRenderer::sendMesh(const std::shared_ptr<jleMesh> &mesh, const glm::mat4 &transform, int instanceId)
{
    _queuedMeshes.push_back({transform, mesh, instanceId});
}

void
jle3DRenderer::renderMeshes(const jleCamera &camera, const std::vector<jle3DRendererQueuedMesh> &meshes)
{
    JLE_SCOPE_PROFILE(jle3DRenderer::renderMeshes)

    if (meshes.empty()) {
        return;
    }

    // Bind to shadow map texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _shadowMappingFramebuffer->texture());
    jleStaticOpenGLState::globalActiveTexture = _shadowMappingFramebuffer->texture();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _pointsShadowMappingFramebuffer->texture());

    _defaultMeshShader.use();
    _defaultMeshShader.SetInt("shadowMap", 0);
    _defaultMeshShader.SetInt("shadowMapPoint", 1); // TODO: multi textures here

    _defaultMeshShader.SetFloat("farPlane", 500.f);

    _defaultMeshShader.SetBool("UseDirectionalLight", _useDirectionalLight);
    _defaultMeshShader.SetVec3("DirectionalLightColour", _directionalLightColour);
    _defaultMeshShader.SetVec3("DirectionalLightDir", _directionalLightRotation);

    _defaultMeshShader.SetMat4("view", camera.getViewMatrix());
    _defaultMeshShader.SetMat4("proj", camera.getProjectionMatrix());
    _defaultMeshShader.SetMat4("lightSpaceMatrix", _lightSpaceMatrix);
    _defaultMeshShader.SetVec3("CameraPosition", camera.getPosition());
    _defaultMeshShader.SetInt("LightsCount", (int)_queuedLights.size());

    if (_queuedLights.size() > 4) // Limit to 4 lights
    {
        _queuedLights.erase(_queuedLights.begin() + 4, _queuedLights.end());
    }

    for (int l = 0; l < _queuedLights.size(); l++) {
        _defaultMeshShader.SetVec3("LightPositions[" + std::to_string(l) + "]", _queuedLights[l].position);
        _defaultMeshShader.SetVec3("LightColors[" + std::to_string(l) + "]", _queuedLights[l].color);
    }

    for (auto &&mesh : meshes) {
        _defaultMeshShader.SetMat4("model", mesh.transform);
        glBindVertexArray(mesh.mesh->getVAO());
        if (mesh.mesh->usesIndexing()) {
            glDrawElements(GL_TRIANGLES, mesh.mesh->getTrianglesCount(), GL_UNSIGNED_INT, (void *)0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, mesh.mesh->getTrianglesCount());
        }
        glBindVertexArray(0);
    }

    glActiveTexture(GL_TEXTURE0);
}
void
jle3DRenderer::setSkybox(const std::shared_ptr<jleSkybox> &skybox)
{
    _skybox = skybox;
}
void
jle3DRenderer::renderSkybox(const jleCamera &camera)
{
    if (!_skybox) {
        return;
    }

    if (camera.getProjectionType() == jleCameraProjection::Orthographic) {
        return;
    }

    // Depth testing to draw the skybox behind everything.
    // We also draw the skybox last, such that it can be early-depth-tested.
    glDepthFunc(GL_LEQUAL);
    _skyboxShader.use();

    // Convert the view matrix to mat3 first to remove the translation
    auto view = glm::mat4(glm::mat3(camera.getViewMatrix()));

    _skyboxShader.SetMat4("view", view);
    _skyboxShader.SetMat4("projection", camera.getProjectionMatrix());
    // skybox cube
    glBindVertexArray(_skybox->getVAO());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _skybox->getTextureID());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
}
void
jle3DRenderer::sendLight(const glm::vec3 &position, const glm::vec3 &color)
{
    _queuedLights.push_back({position, color});
}

void
jle3DRenderer::renderMeshesPicking(jleFramebufferInterface &framebufferOut, const jleCamera &camera)
{
    JLE_SCOPE_PROFILE(jle3DRenderer::renderMeshesPicking)

    const int viewportWidth = framebufferOut.width();
    const int viewportHeight = framebufferOut.height();

    framebufferOut.bind();

    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Change viewport dimensions to match framebuffer's dimensions
    glViewport(0, 0, viewportWidth, viewportHeight);

    _pickingShader.use();
    _pickingShader.SetMat4("projView", camera.getProjectionViewMatrix());

    for (auto &&mesh : _queuedMeshes) {
        int r = (mesh.instanceId & 0x000000FF) >> 0;
        int g = (mesh.instanceId & 0x0000FF00) >> 8;
        int b = (mesh.instanceId & 0x00FF0000) >> 16;
        _pickingShader.SetVec4("PickingColor", glm::vec4{r / 255.0f, g / 255.0f, b / 255.0f, 1.f});
        _pickingShader.SetMat4("model", mesh.transform);
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
jle3DRenderer::renderDirectionalLight(const jleCamera &camera)
{
    JLE_SCOPE_PROFILE(jle3DRenderer::renderDirectionalLight)

    if(!_useDirectionalLight)
    {
        return;
    }

    _shadowMappingFramebuffer->bind();

    glDisable(GL_CULL_FACE);

    _shadowMappingShader.use();

    _shadowMappingShader.SetMat4("lightSpaceMatrix", _lightSpaceMatrix);

    glViewport(0, 0, (int)_shadowMappingFramebuffer->width(), (int)_shadowMappingFramebuffer->height());

    glClear(GL_DEPTH_BUFFER_BIT);
    renderShadowMeshes(_queuedMeshes, _shadowMappingShader);

    glEnable(GL_CULL_FACE);

    _shadowMappingFramebuffer->bindDefault();
}

void
jle3DRenderer::renderPointLights(const jleCamera &camera)
{
    JLE_SCOPE_PROFILE(jle3DRenderer::renderPointLights)

    if (_queuedLights.empty()) {
        return;
    }

    glCullFace(GL_BACK);

    float aspect = (float)_pointsShadowMappingFramebuffer->width() / (float)_pointsShadowMappingFramebuffer->height();
    float near = 0.0f;
    float far = 500.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);

    glm::vec3 lightPos = _queuedLights[0].position;

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

    _shadowMappingPointShader.use();
    _shadowMappingPointShader.SetVec3("lightPos", lightPos);
    _shadowMappingPointShader.SetFloat("farPlane", far);

    for (int i = 0; i < 6; i++) {
        _shadowMappingPointShader.SetMat4("lightSpaceMatrix", shadowTransforms[i]);
        _pointsShadowMappingFramebuffer->setRenderFace(i);
        glViewport(0, 0, (int)_pointsShadowMappingFramebuffer->width(), (int)_pointsShadowMappingFramebuffer->height());

        glClear(GL_DEPTH_BUFFER_BIT);
        renderShadowMeshes(_queuedMeshes, _shadowMappingPointShader);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void
jle3DRenderer::renderShadowMeshes(const std::vector<jle3DRendererQueuedMesh> &meshes, jleShader &shader)
{
    for (auto &&mesh : meshes) {
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
jle3DRenderer::renderFullscreenQuad()
{
    static unsigned int quadVAO = 0;
    static unsigned int quadVBO;

    if (quadVAO == 0) {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
void
jle3DRenderer::setDirectionalLight(const glm::mat4 &view, const glm::vec3 &colour)
{

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(view, scale, rotation, translation, skew, perspective);

    _directionalLightRotation = rotation * glm::vec3{0.f, 0.f, 1.f};
    _directionalLightColour = colour;

    glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
    glm::mat4 lightView =
        glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), rotation * glm::vec3{0.f, -1.f, 0.f}, glm::vec3(0.0f, -1.0f, 0.0f));
    lightView[1][1] *= -1.f;

    _lightSpaceMatrix = lightProjection * view;
}
void
jle3DRenderer::enableDirectionalLight()
{
    _useDirectionalLight = true;
}
void
jle3DRenderer::disableDirectionalLight()
{
    _useDirectionalLight = false;
}
