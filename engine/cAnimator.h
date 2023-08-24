// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleAnimation.h"
#include "jleComponent.h"

#include "jleAnimationFinalMatrices.h"

#include <glm/glm.hpp>

class cAnimator : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cAnimator)
public:
    explicit cAnimator(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void serialize(Archive &ar);

    void start() override;

    void update(float dt) override;

    void editorUpdate(float dt) override;

    void registerLua(sol::state& lua, sol::table &table) override;

    void setAnimation(const jleResourceRef<jleAnimation>& animation);

    void calculateBoneTransform(const jleAnimationNode& node, const glm::mat4& parentTransform);

private:
    jleResourceRef<jleAnimation> _currentAnimation;
    std::shared_ptr<jleAnimationFinalMatrices> _animationMatrices{};
    float _currentTime{};
    float _deltaTime{};
};

JLE_EXTERN_TEMPLATE_CEREAL_H(cAnimator)

CEREAL_REGISTER_TYPE(cAnimator)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cAnimator)
