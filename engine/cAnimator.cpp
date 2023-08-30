// Copyright (c) 2023. Johan Lind

#include "cAnimator.h"

#include "cSkinnedMesh.h"
#include "jle3DGraph.h"
#include "jleProfiler.h"
#include "jleResourceRef.h"

#include "editor/jleEditor.h"

#include <glm/gtc/quaternion.hpp>

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cAnimator)

template <class Archive>
void
cAnimator::serialize(Archive &ar)
{
    try {
        ar(CEREAL_NVP(_currentAnimation),
           CEREAL_NVP(_animationSpeed),
           CEREAL_NVP(_enableRootMotion),
           CEREAL_NVP(_rootMotionBone));
    } catch (std::exception &e) {
        LOGE << "Failed loading cAnimator:" << e.what();
    }
}

cAnimator::cAnimator(jleObject *owner, jleScene *scene) : jleComponent(owner, scene)
{
    _animationMatrices = std::make_shared<jleAnimationFinalMatrices>();
}

void
cAnimator::start()
{
}

void
cAnimator::update(float dt)
{
    JLE_SCOPE_PROFILE_CPU(cAnimator_Skinning)
    dt *= _animationSpeed;
    _deltaTime = dt;
    if (_currentAnimation) {
        _currentTime += _currentAnimation->getTicksPerSec() * dt;
        if (_currentTime / _currentAnimation->getDuration() > 1.f) {
            _animationLoopedThisFrame = true;
        } else {
            _animationLoopedThisFrame = false;
        }
        _currentTime = fmod(_currentTime, _currentAnimation->getDuration());
        calculateBoneTransform(_currentAnimation->getRootNode(), glm::identity<glm::mat4>());
    }
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
}

void
cAnimator::calculateBoneTransform(const jleAnimationNode &node, const glm::mat4 &parentTransform)
{
    const std::string &nodeName = node.name;
    glm::mat4 nodeTransform = node.transformation;

    jleAnimationBone *bone = _currentAnimation->findBone(nodeName);

    if (bone) {
        bone->update(_currentTime);
        nodeTransform = bone->getLocalTransform();

        if (!gEngine->isGameKilled()) {

            if (bone->getName() == _rootMotionBone && _enableRootMotion) {
                auto newPos = glm::vec3(bone->getLocalTransform()[3]);

                auto rootMotionDiff = _lastFrameRootPosition - newPos;
                if (!_animationLoopedThisFrame) {
                    glm::mat4 matrix = glm::mat4(glm::vec4(getTransform().getRight(), 0.0f),
                                                 glm::vec4(getTransform().getUp(), 0.0f),
                                                 glm::vec4(getTransform().getForward(), 0.0f),
                                                 glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
                    auto q = glm::quat(matrix);

                    glm::vec3 rotatedVec = -(q * rootMotionDiff);

                    // gEngine->renderGraph().sendLine(getTransform().getWorldPosition(),
                    //                                 getTransform().getWorldPosition() +
                    //                                     glm::normalize(rotatedVec) * 5.f);

                    glm::vec3 scale;
                    auto modelMatrix = getTransform().getWorldMatrix();
                    scale.x = glm::length(glm::vec3(modelMatrix[0]));
                    scale.y = glm::length(glm::vec3(modelMatrix[1]));
                    scale.z = glm::length(glm::vec3(modelMatrix[2]));

                    getTransform().addLocalTranslation(rotatedVec * scale);
                    for (auto &child : object()->childObjects()) {
                        child->getTransform().addLocalTranslation(rootMotionDiff);
                    }
                    _lastFrameRootPosition = newPos;

                } else {
                    _lastFrameRootPosition = glm::vec3{0.f, _lastFrameRootPosition.y, 0.f};
                    for (auto &child : object()->childObjects()) {
                        child->getTransform().setLocalPosition(glm::vec3{0.f});
                    }
                }
            }
        }
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    auto boneMapping = _currentAnimation->getBoneMapping();
    auto meshBone = boneMapping.find(nodeName);
    if (meshBone != boneMapping.end()) {
        int index = meshBone->second.index;
        const glm::mat4 &offset = meshBone->second.offset;
        _animationMatrices->matrices[index] = globalTransformation * offset;
    }

    for (int i = 0; i < node.childNodes.size(); ++i) {
        calculateBoneTransform(node.childNodes[i], globalTransformation);
    }
}

void
cAnimator::setAnimation(const jleResourceRef<jleAnimation> &animation)
{
    _currentAnimation = animation;
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
    if (_currentAnimation) {
        if (gEditor->isGameKilled()) {
            ImGui::Checkbox("Preview Animation", &_editorPreviewAnimation);
            if (ImGui::IsItemEdited()) {
                if (!_editorPreviewAnimation) {
                    _animationMatrices = std::make_shared<jleAnimationFinalMatrices>();
                }
            }
        }
        ImGui::SliderFloat("Current Time", &_currentTime, 0.f, _currentAnimation->getDuration());
    }
#endif
}
