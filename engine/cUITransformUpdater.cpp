// Copyright (c) 2023. Johan Lind

#include "cUITransformUpdater.h"
#include "jleGameEngine.h"

cUITransformUpdater::cUITransformUpdater(jleObject *owner, jleScene *scene)
    : jleComponent(owner, scene) {}

cUITransformUpdater::~cUITransformUpdater() {}

void cUITransformUpdater::start() {

    jleObject *parent = _attachedToObject;
    do {

        if (auto &&c = parent->getComponent<cCamera>()) {
            _camera = c;
            break;
        }
        parent = parent->parent();

    } while (parent);
}

void cUITransformUpdater::update(float dt) {
    // TODO: dont do this in update, rely on callback functions instead
    // since the UI gets dirty every frame now...

    const auto framebuffer = ((jleGameEngine *)(gCore))->mainScreenFramebuffer;

    getTransform().setLocalPosition({_x, _y, 0.f});

    if (_top) {
        getTransform().setLocalPosition({_x, _y, 0.f});
    }
    else if (_bottom) {
        getTransform().setLocalPosition({_x, framebuffer->height() + _y, 0.f});
    }

    if (_left) {
        getTransform().setLocalPosition({_x, _y, 0.f});
    }
    else if (_right) {
        getTransform().setLocalPosition({framebuffer->width() + _x, _y, 0.f});
    }
}
