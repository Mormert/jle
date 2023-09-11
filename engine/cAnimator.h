// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleAnimation.h"
#include "jleComponent.h"
#include "jleResourceRef.h"

#include "jleAnimationFinalMatrices.h"

#include <glm/glm.hpp>

struct cAnimatorAnimation
{
    jleResourceRef<jleAnimation> currentAnimation{};
    jleAnimation currentAnimationLocal;
    jleAnimationFinalMatrices animationMatrices{};

    float currentTime{};
    float deltaTime{};
    float animationSpeed{1.f};
    bool animationLoopedThisFrame{false};

    float blendFactorStrength = 1.f;

    glm::vec3 lastFrameRootPosition;

    // Local bone root motion translation
    glm::vec3 thisFrameRootMotionTranslation;

    // Local model root motion translation, additive
    glm::vec3 totalRootMotionTranslation;

    template <class Archive>
    void serialize(Archive &ar);

};

JLE_EXTERN_TEMPLATE_CEREAL_H(cAnimatorAnimation)


class cAnimator : public jleComponent, public std::enable_shared_from_this<cAnimator>
{
    JLE_REGISTER_COMPONENT_TYPE(cAnimator)
public:
    explicit cAnimator(jleObject *owner = nullptr, jleScene *scene = nullptr);

    template <class Archive>
    void serialize(Archive &ar);

    void start() override;

    void update(float dt) override;

    void parallelUpdate(float dt) override;

    void editorUpdate(float dt) override;

    void registerLua(sol::state& lua, sol::table &table) override;

    void calculateBoneTransform(const jleAnimationNode& node, const glm::mat4& parentTransform, cAnimatorAnimation& animation);

    const std::shared_ptr<jleAnimationFinalMatrices>& animationMatrices();

    void editorInspectorImGuiRender() override;

private:

    void blendAnimations();

    void applyRootMotion();

    std::vector<cAnimatorAnimation> _animations;

    std::shared_ptr<jleAnimationFinalMatrices> _animationMatrices{};

    bool _enableRootMotion{false};
    std::string _rootMotionBone{"RootBone"};

    float _blendFactor{1.f};

#ifdef BUILD_EDITOR
    bool _editorPreviewAnimation{false};
#endif
};

JLE_EXTERN_TEMPLATE_CEREAL_H(cAnimator)

CEREAL_REGISTER_TYPE(cAnimator)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cAnimator)
