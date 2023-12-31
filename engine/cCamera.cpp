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
#include "jleGame.h"
#include "jleGameEngine.h"
#include "jleIncludeGL.h"
#include "jleObject.h"
#include "jleWindow.h"

#ifdef JLE_BUILD_EDITOR
#include "editor/jleEditor.h"
#include "editor/jleEditorGizmos.h"
#include "jle3DGraph.h"
#include <ImGui/imgui.h>
#endif

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cCamera)

void
cCamera::start()
{
    sInstanceCounter++;

    if (sInstanceCounter > 1) {
        LOG_ERROR << "More than one camera detected!";
    }

    _framebufferCallbackId = gEngine->addGameWindowResizeCallback([this](auto &&PH1, auto &&PH2) {
        framebufferResizeCallback(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    });

    framebufferResizeCallback(gEngine->window().width(), gEngine->window().height());
}

void
cCamera::framebufferResizeCallback(unsigned int width, unsigned int height)
{
    glm::ivec2 dimensions{width, height};
    if (_framebufferUseFixedAxis) {
        if (_framebufferFixedAxis == jleFramebufferInterface::FIXED_AXIS::width) {
            const auto aspect = static_cast<float>(height) / static_cast<float>(width);
            dimensions = jleFramebufferInterface::fixedAxisDimensions(_framebufferFixedAxis, aspect, _framebufferSizeX);
        } else if (_framebufferFixedAxis == jleFramebufferInterface::FIXED_AXIS::height) {
            const auto aspect = static_cast<float>(width) / static_cast<float>(height);

            dimensions = jleFramebufferInterface::fixedAxisDimensions(_framebufferFixedAxis, aspect, _framebufferSizeY);
        }
    } else {
        dimensions = {width, height};
    }

    if (_matchFramebufferToWindowSize) {
        dimensions = {width, height};
    }

    gEngine->resizeMainFramebuffer(dimensions.x, dimensions.y);
}

void
cCamera::update(float dt)
{
    auto &game = ((jleGameEngine *)gEngine)->gameRef();

    auto width = gEngine->mainScreenFramebuffer->width();
    auto height = gEngine->mainScreenFramebuffer->height();

    if (_perspective && width > 0 && height > 0) {
        game.mainCamera.setPerspectiveProjection(_perspectiveFov, width, height, _farPlane, _nearPlane);
    } else {
        game.mainCamera.setOrthographicProjection(width, height, _farPlane, _nearPlane);
    }

    jleCameraSimpleFPVController c;
    c.position = getTransform().getWorldPosition();

    auto &&transformation = _attachedToObject->getTransform().getWorldMatrix();
    game.mainCamera.setViewMatrix(glm::inverse(transformation), c.position);
}

cCamera::cCamera(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

cCamera::~cCamera()
{
    sInstanceCounter--;
    if (gEngine)
        gEngine->removeGameWindowResizeCallback(_framebufferCallbackId);
}

void
cCamera::editorInspectorImGuiRender()
{
#ifdef JLE_BUILD_EDITOR
    ImGui::Text("Camera Preview");

    // Get the texture from the framebuffer
    auto fb = gEngine->mainScreenFramebuffer;
    glBindTexture(GL_TEXTURE_2D, (unsigned int)fb->texture());
    ImGui::Image(
        (void *)(intptr_t)fb->texture(), ImVec2(fb->width() / 4.f, fb->height() / 4.f), ImVec2(0, 1), ImVec2(1, 0));

#endif
}
void
cCamera::editorGizmosRender(bool selected)
{
#ifdef JLE_BUILD_EDITOR
    auto mesh = gEditor->gizmos().cameraMesh();
    auto material = gEditor->gizmos().cameraMaterial();
    gEngine->renderGraph().sendMesh(
        mesh, material, getTransform().getWorldMatrix(), _attachedToObject->instanceID(), false);
#endif // JLE_BUILD_EDITOR
}
