// Copyright (c) 2022. Johan Lind

#include "cText.h"
#include "jleGameEngine.h"
#include "jleObject.h"
#include "jlePathDefines.h"
#include "jleResourceHolder.h"

cText::cText(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

void cText::Start() {
    _transform = _attachedToObject->AddDependencyComponent<cTransform>(this);
}

void cText::ToJson(nlohmann::json& j_out) {
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

void cText::FromJson(const nlohmann::json& j_in) {
    _fontPath = j_in.at("fontPath");
    _text = j_in.at("text");
    _fontSize = j_in.at("fontSize");
    _colorR = j_in.at("colorR");
    _colorG = j_in.at("colorG");
    _colorB = j_in.at("colorB");
    _colorA = j_in.at("colorA");

    _font = jleResourceHolder::LoadResourceFromFile<jleFont>(
        jleRelativePath{_fontPath});
}

void cText::Update(float dt) {
    if (!_font) {
        return;
    }

    jleCore::core->rendering->texts().SendFontText(_font.get(),
                                                   _text,
                                                   _fontSize,
                                                   _transform->GetWorldX(),
                                                   _transform->GetWorldY(),
                                                   _transform->GetWorldDepth(),
                                                   _colorR,
                                                   _colorG,
                                                   _colorB,
                                                   _colorA);
}

void cText::SetText(const std::string& text) { _text = text; }
