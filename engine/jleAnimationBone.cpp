// Copyright (c) 2023. Johan Lind

#include "jleAnimationBone.h"
#include <plog/Log.h>
jleAnimationBone::jleAnimationBone(const std::string &name, int id, const aiNodeAnim *channel)
    : _name(name) , _id(id), _localTransform(1.f)
{
    int numPositions = channel->mNumPositionKeys;
    for(int i = 0; i < numPositions; ++i){
        auto assimpPosition = channel->mPositionKeys[i].mValue;
        float timeStamp = channel->mPositionKeys[i].mTime;
        jleAnimationKeyPosition keyPosition;
        keyPosition.position = glm::vec3{assimpPosition.x, assimpPosition.y, assimpPosition.z};
        keyPosition.timeStamp = timeStamp;
        _positions.push_back(keyPosition);
    }

    int numRotations = channel->mNumRotationKeys;
    for (int i = 0; i < numRotations; ++i) {
        auto assimpQuat = channel->mRotationKeys[i].mValue;
        float timeStamp = channel->mRotationKeys[i].mTime;
        jleAnimationKeyRotation keyRotation;
        keyRotation.orientation = glm::quat{assimpQuat.w, assimpQuat.x, assimpQuat.y, assimpQuat.z};
        keyRotation.timeStamp = timeStamp;
        _rotations.push_back(keyRotation);
    }

    int numScales = channel->mNumScalingKeys;
    for(int i = 0; i < numScales; ++i){
        auto assimpScale = channel->mScalingKeys[i].mValue;
        float timeStamp = channel->mScalingKeys[i].mTime;
        jleAnimationKeyScale keyScale;
        keyScale.scale = glm::vec3{assimpScale.x, assimpScale.y, assimpScale.z};
        keyScale.timeStamp = timeStamp;
        _scales.push_back(keyScale);
    }
}
void
jleAnimationBone::update(float time)
{
    auto translation = lerpPosition(time);
    auto rotation = slerpRotation(time);
    auto scale = lerpScale(time);
    _localTransform = translation * rotation * scale;
}

glm::mat4
jleAnimationBone::getLocalTransform()
{
    return _localTransform;
}

const std::string&
jleAnimationBone::getName() const
{
    return _name;
}

int
jleAnimationBone::getBoneId()
{
    return _id;
}

int
jleAnimationBone::getPositionIndex(float animTime)
{
    for(int i = 0; i < _positions.size() - 1; ++i){
        if(animTime < _positions[i+1].timeStamp){
            return i;
        }
    }

    LOGE << "Failed loading position index";
    return 0;
}

int
jleAnimationBone::getRotationIndex(float animTime)
{    for(int i = 0; i < _rotations.size() - 1; ++i){
        if(animTime < _rotations[i+1].timeStamp){
            return i;
        }
    }

    LOGE << "Failed loading rotation index";
    return 0;
}
int
jleAnimationBone::getScaleIndex(float animTime)
{    for(int i = 0; i < _scales.size() - 1; ++i){
        if(animTime < _scales[i+1].timeStamp){
            return i;
        }
    }

    LOGE << "Failed loading scale index";
    return 0;
}

float
jleAnimationBone::getScaleFactor(float lastTime, float nextTime, float animTime)
{
    float midwayLength = animTime - lastTime;
    float frameDiff = nextTime - lastTime;
    float scaleFactor = midwayLength / frameDiff;
    return scaleFactor;
}

glm::mat4
jleAnimationBone::lerpPosition(float animTime)
{
    if(_positions.size() == 1){
        return glm::translate(glm::mat4{1.f}, _positions[0].position);
    }

    int p0Index = getPositionIndex(animTime);
    int p1Index = p0Index + 1;
    float scaleFactor = getScaleFactor(_positions[p0Index].timeStamp, _positions[p1Index].timeStamp, animTime);
    auto finalPosition = glm::mix(_positions[p0Index].position, _positions[p1Index].position, scaleFactor);
    return glm::translate(glm::mat4{1.f}, finalPosition);
}

glm::mat4
jleAnimationBone::slerpRotation(float animTime)
{
    if(_rotations.size() == 1){
        auto rotation = glm::normalize(_rotations[0].orientation);
        return glm::toMat4(rotation);
    }

    int p0Index = getRotationIndex(animTime);
    int p1Index = p0Index + 1;
    float scaleFactor = getScaleFactor(_rotations[p0Index].timeStamp,
                                       _rotations[p1Index].timeStamp, animTime);
    glm::quat slerpedRotation = glm::slerp(_rotations[p0Index].orientation,
                                         _rotations[p1Index].orientation, scaleFactor);
    slerpedRotation = glm::normalize(slerpedRotation);
    return glm::toMat4(slerpedRotation);
}

glm::mat4
jleAnimationBone::lerpScale(float animTime)
{
    if (_scales.size() == 1){
        return glm::scale(glm::mat4(1.0f), _scales[0].scale);
    }

    int p0Index = getScaleIndex(animTime);
    int p1Index = p0Index + 1;
    float scaleFactor = getScaleFactor(_scales[p0Index].timeStamp,
                                       _scales[p1Index].timeStamp, animTime);
    glm::vec3 finalScale = glm::mix(_scales[p0Index].scale, _scales[p1Index].scale
                                    , scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}
