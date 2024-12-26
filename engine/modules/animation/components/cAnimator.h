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

#include "core/jleResourceRef.h"
#include "modules/animation/jleAnimation.h"

#include "modules/animation/jleAnimationFinalMatrices.h"

#include <glm/glm.hpp>

struct jleEngineUpdateContext;
struct jleEditorUpdateContext;
class cTransform;

namespace sol
{
class state;
}

struct cAnimatorAnimation {
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
    void serialize(Archive &ar){
        ar(CEREAL_NVP(currentAnimation), CEREAL_NVP(animationSpeed));
    }
};

class cAnimator
{
public:
    explicit cAnimator();

    template <class Archive>
    void serialize(Archive &ar){
        ar(CEREAL_NVP(_animations),
           CEREAL_NVP(_enableRootMotion),
           CEREAL_NVP(_rootMotionBone),
           CEREAL_NVP(_blendFactor));
    }

    void start();

    void animate(float dt);

    static void registerLua(sol::state &lua);

    void calculateBoneTransform(const jleAnimationNode &node,
                                const glm::mat4 &parentTransform,
                                cAnimatorAnimation &animation);

    const std::shared_ptr<jleAnimationFinalMatrices> &animationMatrices() const;

    void editorInspectorImGuiRender(jleEditorUpdateContext & ctx);

    void setAnimation(const jlePath &path, jleSerializationContext& ctx);

private:
    void blendAnimations();

    void applyRootMotion(cTransform& transform);

    std::vector<cAnimatorAnimation> _animations;

    std::shared_ptr<jleAnimationFinalMatrices> _animationMatrices{};

    bool _enableRootMotion{false};
    std::string _rootMotionBone{"RootBone"};

    float _blendFactor{1.f};

#if JLE_BUILD_EDITOR
    bool _editorPreviewAnimation{false};
#endif
};
