// Copyright (c) 2023. Johan Lind

#include "cAnimator.h"
#include "jleResourceRef.h"
#include "jle3DGraph.h"
#include "cSkinnedMesh.h"

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cAnimator)

template <class Archive>
void
cAnimator::serialize(Archive &ar)
{
    ar(CEREAL_NVP(_currentAnimation));
}

cAnimator::cAnimator(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {
    _animationMatrices = std::make_shared<jleAnimationFinalMatrices>();
}

void
cAnimator::start()
{

}

void
cAnimator::update(float dt)
{
    if(auto skinnedMeshComponent = _attachedToObject->getComponent<cSkinnedMesh>()){

        _deltaTime = dt;
        if(_currentAnimation){
            _currentTime += _currentAnimation->getTicksPerSec() * dt;
            _currentTime = fmod(_currentTime, _currentAnimation->getDuration());
            calculateBoneTransform(_currentAnimation->getRootNode(), glm::identity<glm::mat4>());
        }

        auto skinnedMesh = skinnedMeshComponent->getMesh();
        auto material = skinnedMeshComponent->getMaterial();
        gEngine->renderGraph().sendSkinnedMesh(skinnedMesh, material, _animationMatrices, getTransform().getWorldMatrix(), _attachedToObject->instanceID(), true);
    }
}

void
cAnimator::editorUpdate(float dt)
{

}

void
cAnimator::registerLua(sol::state &lua, sol::table &table)
{

}

void
cAnimator::calculateBoneTransform(const jleAnimationNode &node, const glm::mat4 &parentTransform)
{
    const std::string& nodeName = node.name;
    glm::mat4 nodeTransform = node.transformation;

    jleAnimationBone* bone = _currentAnimation->findBone(nodeName);

    if(bone){
        bone->update(_currentTime);
        nodeTransform = bone->getLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;


    auto skinnedMeshComponent = _attachedToObject->getComponent<cSkinnedMesh>();
    auto boneMapping = skinnedMeshComponent->getMesh()->getBoneMapping();

    //auto boneMapping = _currentAnimation->getBoneMapping();
    auto meshBone = boneMapping.find(nodeName);
    if(meshBone != boneMapping.end()){
        int index = meshBone->second.index;
        const glm::mat4& offset = meshBone->second.offset;
        _animationMatrices->matrices[index] = globalTransformation * offset;
    }

    for(int i = 0; i < node.childNodes.size(); ++i){
        calculateBoneTransform(node.childNodes[i], globalTransformation);
    }
}

void
cAnimator::setAnimation(const jleResourceRef<jleAnimation> &animation)
{
    _currentAnimation = animation;
}
