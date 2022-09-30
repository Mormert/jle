// Copyright (c) 2022. Johan Lind

#include "cText.h"
#include "jleGameEngine.h"
#include "jleObject.h"
#include "jlePathDefines.h"
#include "jleResourceHolder.h"

cText::cText(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

void cText::Start() {
    mTransform = mAttachedToObject->AddDependencyComponent<cTransform>(this);
}

void cText::ToJson(nlohmann::json& j_out) {
    j_out = nlohmann::json{
        {"fontPath", mFontPath},
        {"text", mText},
        {"fontSize", mFontSize},
        {"colorR", mColorR},
        {"colorG", mColorG},
        {"colorB", mColorB},
        {"colorA", mColorA},
    };
}

void cText::FromJson(const nlohmann::json& j_in) {
    mFontPath = j_in.at("fontPath");
    mText = j_in.at("text");
    mFontSize = j_in.at("fontSize");
    mColorR = j_in.at("colorR");
    mColorG = j_in.at("colorG");
    mColorB = j_in.at("colorB");
    mColorA = j_in.at("colorA");

    mFont = jleResourceHolder::LoadResourceFromFile<jleFont>(
        jleRelativePath{mFontPath});
}

void cText::Update(float dt) {
    if (!mFont) {
        return;
    }

    jleCore::core->rendering->texts->SendFontText(mFont.get(),
                                                  mText,
                                                  mFontSize,
                                                  mTransform->GetWorldX(),
                                                  mTransform->GetWorldY(),
                                                  mTransform->GetWorldDepth(),
                                                  mColorR,
                                                  mColorG,
                                                  mColorB,
                                                  mColorA);
}

void cText::SetText(const std::string& text) { mText = text; }
