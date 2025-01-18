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

#include "cCamera.h"

#include "core/jleCamera.h"
#include "modules/core/components/cTransform.h"

void
cCamera::update(UpdateContext& ctx) const
{
    const auto width = ctx.in.width;
    const auto height = ctx.in.height;

    if (perspective && width > 0 && height > 0) {
        ctx.out.camera.setPerspectiveProjection(perspectiveFov, width, height, farPlane, nearPlane);
    } else {
        ctx.out.camera.setOrthographicProjection(width, height, farPlane, nearPlane);
    }

    auto &&transformation = ctx.in.transform.getLocalMatrix();
    ctx.out.camera.setViewMatrix(glm::inverse(transformation), ctx.in.transform.getPosition());
}

/*
void
cCamera::editorInspectorImGuiRender(jleEditorUpdateContext &ctx)
{
#if JLE_BUILD_IMGUI
    ImGui::Text("Camera Preview");

    // Get the texture from the framebuffer
    auto &fb = ctx.engineUpdateContext.gameRuntime.mainGameScreenFramebuffer;
    glBindTexture(GL_TEXTURE_2D, (unsigned int)fb->texture());
    ImGui::Image(
        (void *)(intptr_t)fb->texture(), ImVec2(fb->width() / 4.f, fb->height() / 4.f), ImVec2(0, 1), ImVec2(1, 0));

#endif

} */

/*
void
cCamera::editorGizmosRender(jleFramePacket &packet, jleEditorGizmos &gizmos)
{
#if JLE_BUILD_EDITOR
    auto mesh = gizmos.cameraMesh();
    auto material = gizmos.cameraMaterial();
    packet.sendMesh(mesh, material, getTransform().getWorldMatrix(), _attachedToObject->instanceID(), false);
#endif // JLE_BUILD_EDITOR

}*/

