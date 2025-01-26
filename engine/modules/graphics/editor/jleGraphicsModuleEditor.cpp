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

#include "jleGraphicsModuleEditor.h"

#include <jleEditorGizmos.h>
#include <editor/serialization/jleEditorECSRegistration.h>
#include <game/jleGame.h>
#include <game/jleGameRuntime.h>
#include <modules/hierarchy/jleHierarchyFuncs.h>
#include <modules/hierarchy/components/cTransform.h>
#include <modules/graphics/jleGraphics.h>

#include "modules/graphics/runtime/components/cCamera.h"
#include "modules/graphics/runtime/components/cLight.h"
#include "modules/graphics/runtime/components/cLightDirectional.h"
#include "modules/graphics/runtime/components/cSkinnedMesh.h"
#include "modules/graphics/runtime/components/cSkybox.h"

#include "modules/graphics/core/jleIncludeGL.h"
#include "modules/graphics/runtime/components/cMeshRenderer.h"

#include <modules/graphics/core/jleFrameBufferInterface.h>


namespace {
    void serializeCameraEditor(jlECS::ComponentContainer *thiz, jleImGuiArchive &archive, int componentIndex, int objectIndex)
    {
        cCamera &cameraComponent = *thiz->getPtr<cCamera>(componentIndex);
        archive(cameraComponent);

        if (auto* editorGraphicsModule = archive.editorCtx.getCurrentModules().getModule<jleGraphicsModuleEditor>()) {
            constexpr unsigned int width = 400;
            constexpr unsigned int height = 400;

            ImGui::Text("Camera Preview");

            if (!editorGraphicsModule->cameraPreviewFramebuffer) {
                editorGraphicsModule->cameraPreviewFramebuffer = std::make_unique<jleFramebufferScreen>(width, height);
            }

            glm::mat4 worldMatrix = jleHierarchyFuncs::getWorldMatrix(thiz->getECS().getObject(objectIndex));

            archive.editorCtx.editorFramePacket.camera.setViewMatrix(glm::inverse(worldMatrix));
            if (cameraComponent.perspective) {
                archive.editorCtx.editorFramePacket.camera.setPerspectiveProjection(cameraComponent.perspectiveFov, width, height, cameraComponent.farPlane, cameraComponent.nearPlane);
            }else {
                archive.editorCtx.editorFramePacket.camera.setOrthographicProjection(cameraComponent.framebufferSizeX, cameraComponent.framebufferSizeY, cameraComponent.farPlane, cameraComponent.nearPlane);
            }

            auto &fb = *editorGraphicsModule->cameraPreviewFramebuffer;

            archive.editorCtx.getCurrentModules().getModule<jleGraphicsModule>()->getGraphics().render(*editorGraphicsModule->cameraPreviewFramebuffer, archive.editorCtx.editorFramePacket);

            // Get the texture from the framebuffer
            glBindTexture(GL_TEXTURE_2D, (unsigned int)fb.texture());
            ImGui::Image((void *)(intptr_t)fb.texture(), ImVec2(width / 2.f, height / 2.f),ImVec2(0, 1), ImVec2(1, 0));
        }
    }
}

void
jleGraphicsModuleEditor::initializeECS(jlECS::ECS &ecs)
{
    {
        jlECS::ComponentRegistrationConfig config{
            .serializeInputF_JSON = jlECS::Serialization::serializeInputT_JSON<cCamera>,
            .serializeOutputF_JSON = jlECS::Serialization::serializeOutputT_JSON<cCamera>,
            .serializeInputF_Binary = jlECS::Serialization::serializeInputT_Binary<cCamera>,
            .serializeOutputF_Binary = jlECS::Serialization::serializeOutputT_Binary<cCamera>,
            .serializeImGuiF = serializeCameraEditor
        };
        ecs.registerComponentType<cCamera>(config);
    }

    registerEditorECSComponent<cLight>(ecs);
    registerEditorECSComponent<cLightDirectional>(ecs);
    registerEditorECSComponent<cMeshRenderer>(ecs);
    registerEditorECSComponent<cSkinnedMesh>(ecs);
    registerEditorECSComponent<cSkybox>(ecs);
}

void
jleGraphicsModuleEditor::updateEditor(jleEditorUpdateContext &ctx, const std::vector<glm::mat4> &worldMatrices)
{
    ZoneScoped;
    auto &ecs = ctx.getCurrentECS();

    // TODO: Set up gizmo meshes for rendering using the new rendering system

    for (auto [objectIndex, _] : ecs.iterateMulti_IncludeObjectIndex<cCamera>()) {
        auto mesh = ctx.gizmos.cameraMesh();
        auto material = ctx.gizmos.cameraMaterial();
        //ctx.editorFramePacket.sendMesh(mesh, material, worldMatrices[objectIndex], objectIndex, false);
    }

    for (auto [objectIndex, _] : ecs.iterateMulti_IncludeObjectIndex<cLight>()) {
        auto mesh = ctx.gizmos.lightLampMesh();
        auto material = ctx.gizmos.lampMaterial();
        //ctx.editorFramePacket.sendMesh(mesh, material, worldMatrices[objectIndex], objectIndex, false);
    }

    for (auto [objectIndex, _] : ecs.iterateMulti_IncludeObjectIndex<cLightDirectional>()) {
        auto mesh = ctx.gizmos.sunMesh();
        auto material = ctx.gizmos.sunMaterial();
        //ctx.editorFramePacket.sendMesh(mesh, material, worldMatrices[objectIndex], objectIndex, false);
    }

    _renderThread->processRenderQueue();
}

void
jleGraphicsModuleEditor::setGameWindowSize(uint32_t width, uint32_t height)
{
    _screenFramebuffer->resize(width, height);
    _msaaFramebuffer->resize(width, height);
}

void
jleGraphicsModuleEditor::display()
{

}
