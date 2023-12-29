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

/*

#include "cUITransformUpdater.h"
#include "jleGameEngine.h"

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cUITransformUpdater)

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

 */