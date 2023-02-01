// Copyright (c) 2022. Johan Lind

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

    // TODO: handle rotation

    game.mainCamera.setViewMatrix(c.getLookAtViewMatrix(), c.position);
}

void
cCamera::toJson(nlohmann::json &j_out)
{
    j_out = nlohmann::json{
        {"perspective", _perspective},
        {"farPlane", _farPlane},
        {"nearPlane", _nearPlane},
        {"fov", _perspectiveFov},
        {"framebufferSizeX", _framebufferSizeX},
        {"framebufferSizeY", _framebufferSizeY},
        {"framebufferFixedAxis", _framebufferFixedAxis},
        {"framebufferUseFixedAxis", _framebufferUseFixedAxis},
        {"matchFramebufferToWindowSize", _matchFramebufferToWindowSize},
    };
}

void
cCamera::fromJson(const nlohmann::json &j_in)
{
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _perspective, "perspective", false);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _farPlane, "farPlane", 10000.f);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _nearPlane, "nearPlane", 0.1f);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _perspectiveFov, "fov", 90.f);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _framebufferSizeY, "framebufferSizeY", 1024);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _framebufferUseFixedAxis, "framebufferUseFixedAxis", false);
    JLE_FROM_JSON_WITH_DEFAULT(
        j_in, _framebufferFixedAxis, "framebufferFixedAxis", jleFramebufferInterface::FIXED_AXIS::width);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _framebufferUseFixedAxis, "framebufferUseFixedAxis", false);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _matchFramebufferToWindowSize, "matchFramebufferToWindowSize", false);
}

cCamera::cCamera(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

cCamera::~cCamera()
{
    sInstanceCounter--;
    gEngine->removeGameWindowResizeCallback(_framebufferCallbackId);
}
