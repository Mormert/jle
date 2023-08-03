// Copyright (c) 2023. Johan Lind

#include "cText.h"
#include "jleGameEngine.h"
#include "jleObject.h"
#include "jlePathDefines.h"
#include "jleResource.h"


cText::cText(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

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
