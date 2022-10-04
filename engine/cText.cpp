// Copyright (c) 2022. Johan Lind

#include "cText.h"
#include "jleGameEngine.h"
#include "jleObject.h"
#include "jlePathDefines.h"
#include "jleResourceHolder.h"

cText::cText(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

void cText::start() {
    _transform = _attachedToObject->addDependencyComponent<cTransform>(this);
}

void cText::toJson(nlohmann::json &j_out) {
    j_out = nlohmann::json{
        {"fontPath", _fontPath},
        {"text", _text},
        {"fontSize", _fontSize},
        {"colorR", _colorR},
        {"colorG", _colorG},
        {"colorB", _colorB},
        {"colorA", _colorA},
    };
}

void cText::fromJson(const nlohmann::json &j_in) {
    _fontPath = j_in.at("fontPath");
    _text = j_in.at("text");
    _fontSize = j_in.at("fontSize");
    _colorR = j_in.at("colorR");
    _colorG = j_in.at("colorG");
    _colorB = j_in.at("colorB");
    _colorA = j_in.at("colorA");

    _font = jleResourceHolder::loadResourceFromFile<jleFont>(
        jleRelativePath{_fontPath});
}

void cText::update(float dt) {
    if (!_font) {
        return;
    }

    jleCore::core->rendering->texts().sendFontText(_font.get(),
                                                   _text,
                                                   _fontSize,
                                                   _transform->worldX(),
                                                   _transform->worldY(),
                                                   _transform->worldDepth(),
                                                   _colorR,
                                                   _colorG,
                                                   _colorB,
                                                   _colorA);
}

void cText::text(const std::string &text) { _text = text; }
