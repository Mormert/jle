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

#include "cText.h"
#include "jleGameEngine.h"
#include "jleObject.h"
#include "jlePathDefines.h"
#include "jleResource.h"

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cText)


void cText::start() {}

void cText::update(float dt) {
    if (!_font) {
        return;
    }

    gEngine->textRendering().sendFontText(_font.get(),
                                        _text,
                                        _fontSize,
                                        getTransform().getWorldPosition().x,
                                        getTransform().getWorldPosition().y,
                                        getTransform().getWorldPosition().z,
                                        _colorR,
                                        _colorG,
                                        _colorB,
                                        _colorA);
}

void cText::text(const std::string &text) { _text = text; }
