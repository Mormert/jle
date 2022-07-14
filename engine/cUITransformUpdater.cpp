// Copyright (c) 2022. Johan Lind

#include "cUITransformUpdater.h"
#include "jleGameEngine.h"

jle::cUITransformUpdater::cUITransformUpdater(jle::jleObject *owner, jle::jleScene *scene)
        : jleComponent(owner, scene), mGameRef{((jleGameEngine *) jleCore::core)->GetGameRef()} {
}

jle::cUITransformUpdater::~cUITransformUpdater() {

}

void jle::cUITransformUpdater::Start() {
    mTransform = mAttachedToObject->AddDependencyComponent<cTransform>(this);

    jleObject *parent = mAttachedToObject;
    do {

        if (auto &&c = parent->GetComponent<jle::cCamera>()) {
            mCamera = c;
            break;
        }
        parent = parent->GetParent();

    } while (parent);


}

void jle::cUITransformUpdater::ToJson(nlohmann::json &j_out) {
    j_out["mTop"] = mTop;
    j_out["mBottom"] = mBottom;
    j_out["mLeft"] = mLeft;
    j_out["mRight"] = mRight;
    j_out["mX"] = mX;
    j_out["mY"] = mY;
}

void jle::cUITransformUpdater::FromJson(const nlohmann::json &j_in) {
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mTop, "mTop", true);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mBottom, "mBottom", false);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mLeft, "mLeft", false);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mRight, "mRight", false);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mX, "mX", false);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, mY, "mY", false);
}

void jle::cUITransformUpdater::Update(float dt) {
    // TODO: dont do this in update, rely on callback functions instead
    // since the UI gets dirty every frame now...

    int offsetX{0}, offsetY{0};
    if(auto c = mCamera.lock())
    {
        offsetX = (int)c->GetOffsetX();
        offsetY = (int)c->GetOffsetY();
    }

    mTransform->SetLocalPositionX(mX);
    mTransform->SetLocalPositionY(mY);

    if (mTop) {
        mTransform->SetLocalPositionY(mY + offsetY);
    } else if (mBottom) {
        mTransform->SetLocalPositionY(mGameRef.mMainCamera.mCameraHeight + mY + offsetY);
    }

    if (mLeft) {
        mTransform->SetLocalPositionX(mX + offsetX);
    } else if (mRight) {
        mTransform->SetLocalPositionX(mGameRef.mMainCamera.mCameraWidth + mX + offsetX);
    }

}
