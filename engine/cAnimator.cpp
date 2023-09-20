// Copyright (c) 2023. Johan Lind

#include "cAnimator.h"

#include "cSkinnedMesh.h"
#include "jle3DGraph.h"
#include "jleProfiler.h"
#include "jleResourceRef.h"

#include "editor/jleEditor.h"

#include <execution>
#include <glm/gtc/quaternion.hpp>

#include "jleGame.h"

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cAnimatorAnimation)
JLE_EXTERN_TEMPLATE_CEREAL_CPP(cAnimator)

template <class Archive>
void
cAnimatorAnimation::serialize(Archive &ar)
{
    try {
        ar(CEREAL_NVP(currentAnimation), CEREAL_NVP(animationSpeed));
    } catch (std::exception &e) {
        LOGE << "Failed loading cAnimator: " << e.what();
    }
}

template <class Archive>
void
cAnimator::serialize(Archive &ar)
{
    try {
        ar(CEREAL_NVP(_animations),
           CEREAL_NVP(_enableRootMotion),
           CEREAL_NVP(_rootMotionBone),
           CEREAL_NVP(_blendFactor));
    } catch (std::exception &e) {
        LOGE << "Failed loading cAnimator: " << e.what();
    }
}

cAnimator::cAnimator(jleObject *owner, jleScene *scene) : jleComponent(owner, scene)
{
    _animationMatrices = std::make_shared<jleAnimationFinalMatrices>();

    enableParallelUpdate(4);
}

void
cAnimator::start()
{
    for (auto &animation : _animations) {
        animation.currentAnimationLocal = *animation.currentAnimation.get();
    }
}

void
cAnimator::update(float dt)
{
    /* std::for_each(std::execution::par, _animations.begin(), _animations.end(), [&](cAnimatorAnimation &animation) {
         if (animation.currentAnimation) {
             animation.deltaTime = dt * animation.animationSpeed;
             animation.currentTime += animation.currentAnimation->getTicksPerSec() * animation.deltaTime;
             if (animation.currentTime / animation.currentAnimation->getDuration() > 1.f) {
                 animation.animationLoopedThisFrame = true;
             } else {
                 animation.animationLoopedThisFrame = false;
             }
             animation.currentTime = fmod(animation.currentTime, animation.currentAnimation->getDuration());

             calculateBoneTransform(animation.currentAnimation->getRootNode(), glm::identity<glm::mat4>(), animation);
         }
     });

     blendAnimations();

     applyRootMotion(); */
}

void
cAnimator::parallelUpdate(float dt)
{
    for (auto &animation : _animations) {
        ZoneScopedN("AnimBlendIteration");
        // if (animation.currentAnimationLocal) {
        animation.deltaTime = dt * animation.animationSpeed;
        animation.currentTime += animation.currentAnimationLocal.getTicksPerSec() * animation.deltaTime;
        if (animation.currentTime / animation.currentAnimationLocal.getDuration() > 1.f) {
            animation.animationLoopedThisFrame = true;
        } else {
            animation.animationLoopedThisFrame = false;
        }
        animation.currentTime = fmod(animation.currentTime, animation.currentAnimationLocal.getDuration());

        calculateBoneTransform(animation.currentAnimationLocal.getRootNode(), glm::identity<glm::mat4>(), animation);
        // }
    }

    blendAnimations();

    applyRootMotion();
}

void
cAnimator::editorUpdate(float dt)
{
#ifdef BUILD_EDITOR
    if (_editorPreviewAnimation) {
        update(dt);
    }
#endif
}

void
cAnimator::registerLua(sol::state &lua, sol::table &table)
{
    lua.new_usertype<cAnimator>("cAnimator", sol::base_classes, sol::bases<jleComponent>(), "setAnimation", &cAnimator::setAnimation);
}

void
cAnimator::calculateBoneTransform(const jleAnimationNode &node,
                                  const glm::mat4 &parentTransform,
                                  cAnimatorAnimation &animation)
{
    // ZoneScoped;
    const std::string &nodeName = node.name;
    glm::mat4 nodeTransform = node.transformation;

    jleAnimationBone *bone;
    {
        // ZoneScopedN("FindBone");
        bone = animation.currentAnimationLocal.findBone(nodeName);
    }

    if (bone) {
        // ZoneScopedN("BoneUpdate");
        bone->update(animation.currentTime);
        nodeTransform = bone->getLocalTransform();

        if (!gEngine->isGameKilled()) {
            if (bone->getName() == _rootMotionBone && _enableRootMotion) {
                auto newPos = glm::vec3(bone->getLocalTransform()[3]);

                auto rootMotionDiff = animation.lastFrameRootPosition - newPos;
                animation.lastFrameRootPosition = newPos;

                if (!animation.animationLoopedThisFrame) {
                    animation.thisFrameRootMotionTranslation = rootMotionDiff;
                } else {
                    animation.thisFrameRootMotionTranslation = newPos;
                }
            } else if (!_enableRootMotion) {
                animation.thisFrameRootMotionTranslation = glm::vec3{0.f};
            }
        }
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    {
        // ZoneScopedN("BoneMapping");

        const auto &boneMapping = animation.currentAnimationLocal.getBoneMapping();
        const auto &meshBone = boneMapping.find(nodeName);
        if (meshBone != boneMapping.end()) {
            int index = meshBone->second.index;
            const glm::mat4 &offset = meshBone->second.offset;

            animation.animationMatrices.matrices[index] = globalTransformation * offset;
        }
    }

    for (int i = 0; i < node.childNodes.size(); ++i) {
        calculateBoneTransform(node.childNodes[i], globalTransformation, animation);
    }
}

const std::shared_ptr<jleAnimationFinalMatrices> &
cAnimator::animationMatrices()
{
    return _animationMatrices;
}

void
cAnimator::editorInspectorImGuiRender()
{
#ifdef BUILD_EDITOR
    int p = 0;
    ImGui::Separator();
    for (auto &animation : _animations) {
        ImGui::PushID(p++);
        ImGui::Text("%s", animation.currentAnimationLocal.path.getVirtualPath().c_str());
        // if (animation.currentAnimation) {
        if (gEditor->isGameKilled()) {
            if (ImGui::IsItemEdited()) {
                if (!_editorPreviewAnimation) {
                    _animationMatrices = std::make_shared<jleAnimationFinalMatrices>();
                }
            }
        }
        ImGui::SliderFloat("Current Time", &animation.currentTime, 0.f, animation.currentAnimationLocal.getDuration());
        //}
        ImGui::PopID();
    }

    ImGui::Checkbox("Preview Animation", &_editorPreviewAnimation);

#endif
}

void
cAnimator::blendAnimations()
{
    ZoneScoped;
    if (_animations.size() == 1) {
        // if (_animations[0].currentAnimation) {
        *_animationMatrices = _animations[0].animationMatrices;
        _animations[0].blendFactorStrength = 1.f;
        // }
    } else if (_animations.size() == 2) {
        for (int i = 0; i < 100; ++i) {
            glm::mat4 &matrix1 = _animations[0].animationMatrices.matrices[i];
            glm::mat4 &matrix2 = _animations[1].animationMatrices.matrices[i];
            _animationMatrices->matrices[i] = matrix1 * (1.0f - _blendFactor) + matrix2 * _blendFactor;
        }
        //_animations[0].thisFrameRootMotionTranslation *= 1.0f - _blendFactor;
        //_animations[1].thisFrameRootMotionTranslation *= _blendFactor;
        _animations[0].blendFactorStrength = 1.0f - _blendFactor;
        _animations[1].blendFactorStrength = _blendFactor;
    }
}

void
cAnimator::applyRootMotion()
{
    ZoneScoped;
    glm::mat4 matrix = glm::mat4(glm::vec4(getTransform().getRight(), 0.0f),
                                 glm::vec4(getTransform().getUp(), 0.0f),
                                 glm::vec4(getTransform().getForward(), 0.0f),
                                 glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    auto quat = glm::quat(matrix);

    glm::vec3 scale;
    auto modelMatrix = getTransform().getWorldMatrix();
    scale.x = glm::length(glm::vec3(modelMatrix[0]));
    scale.y = glm::length(glm::vec3(modelMatrix[1]));
    scale.z = glm::length(glm::vec3(modelMatrix[2]));

    for (auto &animation : _animations) {
        if (!animation.animationLoopedThisFrame) {

            glm::vec3 rotatedVec = -(quat * animation.thisFrameRootMotionTranslation * animation.blendFactorStrength);
            animation.totalRootMotionTranslation += rotatedVec;

            // gEngine->renderGraph().sendLine(getTransform().getWorldPosition(),
            //                                getTransform().getWorldPosition() + rotatedVec);

            getTransform().addLocalTranslation(rotatedVec * scale);
        } else {
            animation.totalRootMotionTranslation = glm::vec3{0.f};
        }
    }

    glm::vec3 addedRootMotionForChildren{0.f};
    for (const auto &animation : _animations) {
        addedRootMotionForChildren += animation.totalRootMotionTranslation;
    }

    for (auto &child : object()->childObjects()) {
        child->getTransform().setLocalPosition(addedRootMotionForChildren);
    }
}


void
cAnimator::setAnimation(const jlePath &path)
{
    _animations.clear();
    _animations.push_back({});
    _animations[0].currentAnimation.path = path;
    _animations[0].currentAnimation.loadResource();

    for (auto &animation : _animations) {
        animation.currentAnimationLocal = *animation.currentAnimation.get();
    }
}
