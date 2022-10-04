// Copyright (c) 2022. Johan Lind

#pragma once

#include "cCamera.h"
#include "cTransform.h"
#include "jleComponent.h"
#include "jleGame.h"

class cUITransformUpdater : public jleComponent {
    JLE_REGISTER_COMPONENT_TYPE(cUITransformUpdater)
public:
    explicit cUITransformUpdater(jleObject *owner = nullptr,
                                 jleScene *scene = nullptr);

    ~cUITransformUpdater() override;

    void start() override;

    void toJson(nlohmann::json& j_out) override;

    void fromJson(const nlohmann::json& j_in) override;

    void update(float dt) override;

private:
    std::shared_ptr<cTransform> _transform{nullptr};
    std::weak_ptr<cCamera> _camera{};

    bool _top{true};
    bool _bottom{false};
    bool _left{false};
    bool _right{false};

    int _x{}, _y{};
    jleGame& _gameRef;
};
