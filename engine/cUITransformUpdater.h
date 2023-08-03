// Copyright (c) 2023. Johan Lind

#pragma once

/*

#include "cCamera.h"
#include "jleComponent.h"
#include "jleGame.h"
#include "jleTransform.h"

class cUITransformUpdater : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cUITransformUpdater)
public:
    explicit cUITransformUpdater(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_top),
           CEREAL_NVP(_bottom),
           CEREAL_NVP(_left),
           CEREAL_NVP(_right),
           CEREAL_NVP(_x),
           CEREAL_NVP(_y));
    }

    ~cUITransformUpdater() override;

    void start() override;

    void update(float dt) override;

private:
    std::weak_ptr<cCamera> _camera{};

    bool _top{true};
    bool _bottom{false};
    bool _left{false};
    bool _right{false};

    int _x{}, _y{};
};

JLE_EXTERN_TEMPLATE_CEREAL_H(cUITransformUpdater)

CEREAL_REGISTER_TYPE(cUITransformUpdater)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cUITransformUpdater)

 */