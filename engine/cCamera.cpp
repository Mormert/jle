// Copyright (c) 2023. Johan Lind

#include "cCamera.h"
#include "jleGameEngine.h"
#include "jleObject.h"
#include "jleWindow.h"

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

    framebufferResizeCallback(gCore->window().width(), gCore->window().height());
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
    auto &game = ((jleGameEngine *)gCore)->gameRef();

    if (_perspective) {
        game.mainCamera.setPerspectiveProjection(_perspectiveFov,
                                                 gEngine->mainScreenFramebuffer->width(),
                                                 gEngine->mainScreenFramebuffer->height(),
                                                 _farPlane,
                                                 _nearPlane);
    } else {
        game.mainCamera.setOrthographicProjection(
            gEngine->mainScreenFramebuffer->width(), gEngine->mainScreenFramebuffer->height(), _farPlane, _nearPlane);
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
    if(gEngine)
        gEngine->removeGameWindowResizeCallback(_framebufferCallbackId);
}
