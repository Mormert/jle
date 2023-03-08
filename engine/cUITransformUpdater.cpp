// Copyright (c) 2023. Johan Lind

#include "cUITransformUpdater.h"
#include "jleGameEngine.h"

cUITransformUpdater::cUITransformUpdater(jleObject *owner, jleScene *scene)
    : jleComponent(owner, scene), _gameRef{
                                      ((jleGameEngine *)gCore)->gameRef()} {}

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

void cUITransformUpdater::toJson(nlohmann::json &j_out) {
    j_out["_top"] = _top;
    j_out["_bottom"] = _bottom;
    j_out["_left"] = _left;
    j_out["_right"] = _right;
    j_out["_x"] = _x;
    j_out["_y"] = _y;
}

void cUITransformUpdater::fromJson(const nlohmann::json &j_in) {
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _top, "_top", true);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _bottom, "_bottom", false);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _left, "_left", false);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _right, "_right", false);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _x, "_x", false);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _y, "_y", false);
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
