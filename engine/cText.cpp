// Copyright (c) 2022. Johan Lind

#include "cText.h"
#include "jleResourceHolder.h"
#include "jlePathDefines.h"
#include "jleObject.h"
#include "jleCore.h"

jle::cText::cText(jle::jleObject *owner, jle::jleScene *scene) : jleComponent(owner, scene) {

}

void jle::cText::Start() {
    mTransform = mAttachedToObject->AddDependencyComponent<cTransform>(this);
}

void jle::cText::ToJson(nlohmann::json &j_out) {
    j_out = nlohmann::json{
            {"fontPath", mFontPath},
            {"text",     mText},
            {"fontSize", mFontSize},
            {"colorR",   mColorR},
            {"colorG",   mColorG},
            {"colorB",   mColorB},
            {"colorA",   mColorA},
    };
}

void jle::cText::FromJson(const nlohmann::json &j_in) {
    mFontPath = j_in.at("fontPath");
    mText = j_in.at("text");
    mFontSize = j_in.at("fontSize");
    mColorR = j_in.at("colorR");
    mColorG = j_in.at("colorG");
    mColorB = j_in.at("colorB");
    mColorA = j_in.at("colorA");

    const auto truePath = jle::FindTrueResourcePath(mFontPath);
    mFont = jleResourceHolder<jle::jleFont>::LoadResourceFromFile(truePath);
}

void jle::cText::Update(float dt) {
    if (!mFont) {
        return;
    }

    jle::jleCore::core->rendering->texts->SendFontText(mFont.get(), mText, mFontSize,
                                                       mTransform->GetWorldX(), mTransform->GetWorldY(),
                                                       mTransform->GetWorldDepth(),
                                                       mColorR, mColorG, mColorB, mColorA);
}

void jle::cText::SetText(const std::string &text) {
    mText = text;
}

