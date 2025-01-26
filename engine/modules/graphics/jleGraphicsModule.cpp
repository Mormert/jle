/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#include "jleGraphicsModule.h"

#include "core/jleFramebufferScreen.h"
#include "jleFramePacket.h"
#include "jleGraphics.h"

#include "runtime/components/cCamera.h"
#include "runtime/components/cLight.h"
#include "runtime/components/cLightDirectional.h"
#include "runtime/components/cMeshRenderer.h"
#include "runtime/components/cSkinnedMesh.h"
#include "runtime/components/cSkybox.h"
#include <modules/hierarchy/components/cTransform.h>
#include <modules/mesh/components/cMesh.h>
#include <modules/mesh/jleMeshModule.h>
#include <modules/mesh/jleMesh.h>
#include <modules/image/jleImageModule.h>
#include <core/jleImage.h>
#include "jleMaterial.h"

#include "core/jleIncludeGL.h"
#include "jleSkinnedMesh.h"
#include <jlECS/jlECS.h>

void
jleGraphicsModule::initializeECS(jlECS::ECS &ecs)
{
    ecs.registerComponentType<cCamera>();
    ecs.registerComponentType<cLight>();
    ecs.registerComponentType<cLightDirectional>();
    ecs.registerComponentType<cMeshRenderer>();
    ecs.registerComponentType<cSkinnedMesh>();
    ecs.registerComponentType<cSkybox>();
}

void
jleGraphicsModule::initializeModule(jleSerializationContext &ctx)
{
    _renderThread = std::make_unique<jleRenderThread>();
    ctx.serializationInterfaces.push_back(_renderThread.get());

    _graphics = std::make_unique<jleGraphics>(ctx, this);
    _fullscreen_renderer = std::make_unique<jleFullscreenRendering>(ctx);

    constexpr int initialScreenX = 1024;
    constexpr int initialScreenY = 1024;
    _screenFramebuffer = std::make_unique<jleFramebufferScreen>(initialScreenX, initialScreenY);
    _msaaFramebuffer = std::make_unique<jleFramebufferMultisample>(initialScreenX, initialScreenY, 4);
}

void
jleGraphicsModule::preRender()
{
    getCurrentFramePacket().emptyQueues();
}

void
jleGraphicsModule::render(int windowX, int windowY)
{
    ZoneScoped;

    // Check that the module is initialized
    if (!_renderThread) {
        return;
    }

    for (const auto& mesh : _meshesToLoadIntoGPU) {
        _meshGpuBuffers.push_back({});
        jleMeshGPUData* gpuData = &_meshGpuBuffers.back();
        createMeshGPUBuffers(gpuData, mesh.get());
        jleMeshGPUDataHandle gpuDataHandle {static_cast<uint32_t>(_meshGpuBuffers.size() - 1)};
        _meshGPULookup[mesh->path] = gpuDataHandle;
    }
    _meshesToLoadIntoGPU.clear();

    for (const auto& materialPath : _materialsToLoadIntoGPU) {
        jleSerializationContext ctx{};
        ctx.resources = &_resourceHolder;
        ctx.serializationInterfaces.push_back(_renderThread.get());

        auto material = jleResourceRef<jleMaterial>{materialPath, ctx};

        _materialGpuBuffers.emplace_back();
        jleMaterialGPUData* gpuData = &_materialGpuBuffers.back();
        createMaterialGPUData(gpuData, material.get(), materialPath);
        jleMaterialGPUDataHandle gpuDataHandle {static_cast<uint32_t>(_materialGpuBuffers.size() - 1)};
        _materialGPULookup[materialPath] = gpuDataHandle;
    }
    _materialsToLoadIntoGPU.clear();

    if (_screenFramebuffer->width() != windowX || _screenFramebuffer->height() != windowY) {
        _screenFramebuffer->resize(windowX, windowY);
        _msaaFramebuffer->resize(windowX, windowY);
    }

    _renderThread->processRenderQueue();

    if(!(windowX > 0 && windowY > 0))
    {
        return;
    }

    _graphics->render(*_msaaFramebuffer, getPreviousFramePacket());
    _msaaFramebuffer->blitToOther(*_screenFramebuffer);
    display();
}

void
jleGraphicsModule::postRender()
{
    _currentFramePacketIndex = (_currentFramePacketIndex + 1) % 2;
}

jleGraphicsModule::~jleGraphicsModule() = default;

void
jleGraphicsModule::populateSerializeableInterface(std::vector<jleSerializableInterface *> &interfaces)
{
    if (jleRenderThread *renderThread = _renderThread.get()) {
        interfaces.push_back(renderThread);
    }
}

void
jleGraphicsModule::update(const jleGraphicsModule::UpdateContext &ctx)
{
    const std::vector<glm::mat4> &worldMatrices = ctx.in.worldMatrices;

    auto &framePacket = getCurrentFramePacket();

    for (auto [objectIndex, camera] : ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cCamera>()) {
        cCamera::UpdateContext cameraUpdateCtx = {
            .in = {.transform = worldMatrices[objectIndex], .width = ctx.in.screenX, .height = ctx.in.screenY},
            .out = {.camera = framePacket.camera}};
        camera->update(cameraUpdateCtx);

        // Break here, so we only get one camera
        break;
    }

    for (auto [objectIndex, light] : ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cLight>()) {
        light->ecsUpdate(framePacket, worldMatrices[objectIndex]);
    }

    for (auto [objectIndex, lightDirectional] : ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cLightDirectional>()) {
        lightDirectional->ecsUpdate(framePacket, worldMatrices[objectIndex]);
    }

    for (auto [objectIndex, mesh, meshRenderer] : ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cMesh, cMeshRenderer>()) {
        if (!mesh->getMeshRef())
            continue;
        if (mesh->getMeshRef()->path.isEmpty())
            continue;

        // Check and set GPU mesh
        if (meshRenderer->getGpuMeshPath() == mesh->getMesh()->path) {
            meshRenderer->ecsUpdate(framePacket, worldMatrices[objectIndex], objectIndex, mesh->getMesh()->path, mesh->getMesh());
        } else {
            if (auto it = _meshGPULookup.find(mesh->getMesh()->path); it != _meshGPULookup.end()) {
                meshRenderer->setGpuMesh(mesh->getMesh()->path, it->second);
            } else {
                if (std::shared_ptr<jleMesh> loadedMesh = ctx.in.meshModule->getLoadedMesh(mesh->getMesh()->path)) {
                    _meshesToLoadIntoGPU.insert(loadedMesh);
                }
            }
        }

        // Check and set GPU material
        const jlePath materialPath = meshRenderer->getMaterialPath();
        if (!materialPath.isEmpty()) {
            if (meshRenderer->getGpuMaterialPath() != materialPath) {
                if (auto it = _materialGPULookup.find(materialPath); it != _materialGPULookup.end()) {
                    meshRenderer->setGpuMaterial(materialPath, it->second);
                } else {
                    _materialsToLoadIntoGPU.insert(materialPath);
                }
            }
        }
    }

    // TODO: Add back support for skinned meshes ...
    /*for (auto [objectIndex, skinnedMesh] : ctx.inOut.ecs.iterateMulti_IncludeObjectIndex<cSkinnedMesh>()) {

        auto object = ctx.inOut.ecs.getObject(objectIndex);
        auto optionalAnimator = object.getComponentPtr<cAnimator>();

        if (skinnedMesh->getMesh()) {
            auto gpuData = getOrCreateSkinnedMeshGPU(skinnedMesh->getMesh()->path, skinnedMesh->getMesh().get());
            skinnedMesh->ecsUpdate(framePacket, worldMatrices[objectIndex], optionalAnimator, objectIndex, gpuData);
        } else {
            skinnedMesh->ecsUpdate(framePacket, worldMatrices[objectIndex], optionalAnimator, objectIndex, nullptr);
        }
    }*/

    for (auto &skybox : ctx.inOut.ecs.iterate<cSkybox>()) {
        framePacket.settings.skybox = skybox.getSkyboxRef();
        // Break here so we only get one skybox
        break;
    }
}

void
jleGraphicsModule::display()
{
    _fullscreen_renderer->renderFramebufferFullscreen(*_screenFramebuffer, _screenFramebuffer->width(), _screenFramebuffer->width());
}

void
jleGraphicsModule::destroyMeshGPUBuffers(jleMeshGPUData* gpuData)
{
    if (!gpuData) return;
    
    if (gpuData->vbo_pos != UINT32_MAX) {
        glDeleteBuffers(1, &gpuData->vbo_pos);
        gpuData->vbo_pos = UINT32_MAX;
    }
    if (gpuData->vbo_normal != UINT32_MAX) {
        glDeleteBuffers(1, &gpuData->vbo_normal);
        gpuData->vbo_normal = UINT32_MAX;
    }
    if (gpuData->vbo_texcoords != UINT32_MAX) {
        glDeleteBuffers(1, &gpuData->vbo_texcoords);
        gpuData->vbo_texcoords = UINT32_MAX;
    }
    if (gpuData->vbo_tangent != UINT32_MAX) {
        glDeleteBuffers(1, &gpuData->vbo_tangent);
        gpuData->vbo_tangent = UINT32_MAX;
    }
    if (gpuData->vbo_bitangent != UINT32_MAX) {
        glDeleteBuffers(1, &gpuData->vbo_bitangent);
        gpuData->vbo_bitangent = UINT32_MAX;
    }
    if (gpuData->ebo != UINT32_MAX) {
        glDeleteBuffers(1, &gpuData->ebo);
        gpuData->ebo = UINT32_MAX;
    }
    if (gpuData->vao != UINT32_MAX) {
        glDeleteVertexArrays(1, &gpuData->vao);
        gpuData->vao = UINT32_MAX;
    }
}

void
jleGraphicsModule::createMeshGPUBuffers(jleMeshGPUData* gpuData, const jleMesh* mesh)
{
    if (!gpuData || !mesh) return;

    gpuData->usesIndexing = mesh->usesIndexing();
    gpuData->trianglesCount = mesh->getTrianglesCount();

    glGenVertexArrays(1, &gpuData->vao);
    glBindVertexArray(gpuData->vao);

    if (!mesh->positions().empty()) {
        glGenBuffers(1, &gpuData->vbo_pos);
        glBindBuffer(GL_ARRAY_BUFFER, gpuData->vbo_pos);
        glBufferData(GL_ARRAY_BUFFER, mesh->positions().size() * sizeof(glm::vec3), &mesh->positions()[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        glEnableVertexAttribArray(0);
    }

    if (!mesh->normals().empty()) {
        glGenBuffers(1, &gpuData->vbo_normal);
        glBindBuffer(GL_ARRAY_BUFFER, gpuData->vbo_normal);
        glBufferData(GL_ARRAY_BUFFER, mesh->normals().size() * sizeof(glm::vec3), &mesh->normals()[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        glEnableVertexAttribArray(1);
    }

    if (!mesh->texCoords().empty()) {
        glGenBuffers(1, &gpuData->vbo_texcoords);
        glBindBuffer(GL_ARRAY_BUFFER, gpuData->vbo_texcoords);
        glBufferData(GL_ARRAY_BUFFER, mesh->texCoords().size() * sizeof(glm::vec2), &mesh->texCoords()[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
        glEnableVertexAttribArray(2);
    }

    if (!mesh->tangents().empty()) {
        glGenBuffers(1, &gpuData->vbo_tangent);
        glBindBuffer(GL_ARRAY_BUFFER, gpuData->vbo_tangent);
        glBufferData(GL_ARRAY_BUFFER, mesh->tangents().size() * sizeof(glm::vec3), &mesh->tangents()[0], GL_STATIC_DRAW);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        glEnableVertexAttribArray(3);
    }

    if (!mesh->bitangents().empty()) {
        glGenBuffers(1, &gpuData->vbo_bitangent);
        glBindBuffer(GL_ARRAY_BUFFER, gpuData->vbo_bitangent);
        glBufferData(GL_ARRAY_BUFFER, mesh->bitangents().size() * sizeof(glm::vec3), &mesh->bitangents()[0], GL_STATIC_DRAW);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        glEnableVertexAttribArray(4);
    }

    if (!mesh->indices().empty()) {
        glGenBuffers(1, &gpuData->ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuData->ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices().size() * sizeof(unsigned int), &mesh->indices()[0], GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
}

void
jleGraphicsModule::destroySkinnedMeshGPUBuffers(jleSkinnedMeshGPUData* gpuData)
{
    if (!gpuData) return;

    destroyMeshGPUBuffers(gpuData);

    if (gpuData->vbo_boneIndices != UINT32_MAX) {
        glDeleteBuffers(1, &gpuData->vbo_boneIndices);
        gpuData->vbo_boneIndices = UINT32_MAX;
    }
    if (gpuData->vbo_boneWeights != UINT32_MAX) {
        glDeleteBuffers(1, &gpuData->vbo_boneWeights);
        gpuData->vbo_boneWeights = UINT32_MAX;
    }
}

void
jleGraphicsModule::createSkinnedMeshGPUBuffers(jleSkinnedMeshGPUData* gpuData, jleSkinnedMesh* mesh)
{
    if (!gpuData || !mesh) return;

    createMeshGPUBuffers(gpuData, mesh);

    glBindVertexArray(gpuData->vao);

    if (!mesh->boneIndices().empty()) {
        glGenBuffers(1, &gpuData->vbo_boneIndices);
        glBindBuffer(GL_ARRAY_BUFFER, gpuData->vbo_boneIndices);
        glBufferData(GL_ARRAY_BUFFER, mesh->boneIndices().size() * sizeof(glm::ivec4), &mesh->boneIndices()[0], GL_STATIC_DRAW);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(float) * 4, 0);
        glEnableVertexAttribArray(5);
    }

    if (!mesh->boneWeights().empty()) {
        glGenBuffers(1, &gpuData->vbo_boneWeights);
        glBindBuffer(GL_ARRAY_BUFFER, gpuData->vbo_boneWeights);
        glBufferData(GL_ARRAY_BUFFER, mesh->boneWeights().size() * sizeof(glm::vec4), &mesh->boneWeights()[0], GL_STATIC_DRAW);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
        glEnableVertexAttribArray(6);
    }

    glBindVertexArray(0);
}

void
jleGraphicsModule::createMaterialGPUData(jleMaterialGPUData* gpuData, std::shared_ptr<jleMaterial> material, const jlePath& path)
{
    if (!gpuData) return;

    gpuData->material = std::move(material);
    gpuData->materialPath = path;
}

void
jleGraphicsModule::destroyMaterialGPUData(jleMaterialGPUData* gpuData)
{
    if (!gpuData) return;

    gpuData->material.reset();
    gpuData->materialPath = jlePath{};
}
