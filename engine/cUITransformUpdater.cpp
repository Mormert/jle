// Copyright (c) 2022. Johan Lind

#include "cUITransformUpdater.h"
#include "jleGameEngine.h"

cUITransformUpdater::cUITransformUpdater(jleObject *owner, jleScene *scene)
    : jleComponent(owner, scene),
      _gameRef{((jleGameEngine *)jleCore::core)->GetGameRef()} {}

cUITransformUpdater::~cUITransformUpdater() {}

void cUITransformUpdater::Start() {
    _transform = _attachedToObject->AddDependencyComponent<cTransform>(this);

    jleObject *parent = _attachedToObject;
    do {

        if (auto&& c = parent->GetComponent<cCamera>()) {
            _camera = c;
            break;
        }
        parent = parent->GetParent();

    } while (parent);
}

void cUITransformUpdater::ToJson(nlohmann::json& j_out) {
    j_out["_top"] = _top;
    j_out["_bottom"] = _bottom;
    j_out["_left"] = _left;
    j_out["_right"] = _right;
    j_out["_x"] = _x;
    j_out["_y"] = _y;
}

void cUITransformUpdater::FromJson(const nlohmann::json& j_in) {
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _top, "_top", true);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _bottom, "_bottom", false);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _left, "_left", false);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _right, "_right", false);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _x, "_x", false);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, _y, "_y", false);
}

void cUITransformUpdater::Update(float dt) {
    // TODO: dont do this in update, rely on callback functions instead
    // since the UI gets dirty every frame now...

    int offsetX{0}, offsetY{0};
    if (auto c = _camera.lock()) {
        offsetX = (int)c->GetOffsetX();
        offsetY = (int)c->GetOffsetY();
    }

    _transform->SetLocalPositionX(_x);
    _transform->SetLocalPositionY(_y);

    if (_top) {
        _transform->SetLocalPositionY(_y + offsetY);
    }
    else if (_bottom) {
        _transform->SetLocalPositionY(_gameRef._mainCamera._cameraHeight + _y +
                                      offsetY);
    }

    if (_left) {
        _transform->SetLocalPositionX(_x + offsetX);
    }
    else if (_right) {
        _transform->SetLocalPositionX(_gameRef._mainCamera._cameraWidth + _x +
                                      offsetX);
    }
}
