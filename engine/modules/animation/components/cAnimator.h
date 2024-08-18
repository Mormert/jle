/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#pragma once

#include "core/jleCommon.h"

#include "jleComponent.h"
#include "jleResourceRef.h"
#include "modules/animation/jleAnimation.h"

#include "modules/animation/jleAnimationFinalMatrices.h"

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
    explicit cAnimator();

    template <class Archive>
    void serialize(Archive &ar);

    void start(jleEngineModulesContext& ctx) override;

    void update(jleEngineModulesContext& ctx) override;

    void parallelUpdate(jleEngineModulesContext& ctx) override;

    void editorUpdate(jleEngineModulesContext& ctx) override;

    void registerLua(sol::state& lua) override;

    void calculateBoneTransform(const jleAnimationNode& node, const glm::mat4& parentTransform, cAnimatorAnimation& animation);

    const std::shared_ptr<jleAnimationFinalMatrices>& animationMatrices();

    void editorInspectorImGuiRender() override;

    void setAnimation(const jlePath& path, jleResources& resources);

private:

    void blendAnimations();

    void applyRootMotion();

    std::vector<cAnimatorAnimation> _animations;

    std::shared_ptr<jleAnimationFinalMatrices> _animationMatrices{};

    bool _enableRootMotion{false};
    std::string _rootMotionBone{"RootBone"};

    float _blendFactor{1.f};

#if JLE_BUILD_EDITOR
    bool _editorPreviewAnimation{false};
#endif
};

JLE_EXTERN_TEMPLATE_CEREAL_H(cAnimator)

CEREAL_REGISTER_TYPE(cAnimator)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cAnimator)
