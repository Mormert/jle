// Copyright (c) 2023. Johan Lind

#include "jleAnimation.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/gtc/type_ptr.hpp>

jleLoadFromFileSuccessCode
jleAnimation::loadFromFile(const jlePath &path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.getRealPath(), aiProcess_Triangulate);
    if(!scene || !scene->mRootNode){
        LOGE << "Failed loading animation, no scene or no root node.";
        return jleLoadFromFileSuccessCode::FAIL;
    }

    if(!scene->HasAnimations()){
        LOGE << "Failed loading animation, loaded model is missing animations.";
        return jleLoadFromFileSuccessCode::FAIL;
    }

    auto animation = scene->mAnimations[0];
    _duration = animation->mDuration;
    _ticksPerSecond = animation->mTicksPerSecond;
    readHierarchyData(_rootNode, scene->mRootNode);

    readBones(animation);

    return jleLoadFromFileSuccessCode::SUCCESS;
}
jleAnimation::jleAnimation(const std::string &path, jleSkinnedMesh &mesh) {}

jleAnimationBone *
jleAnimation::findBone(const std::string &name)
{
    auto iter = std::find_if(_bones.begin(), _bones.end(),
                             [&](const jleAnimationBone& Bone)
                             {
                                 return Bone.getName() == name;
                             }
    );
    if (iter == _bones.end()) return nullptr;
    else return &(*iter);
}
float
jleAnimation::getTicksPerSec()
{
    return _ticksPerSecond;
}
float
jleAnimation::getDuration()
{
    return _duration;
}
jleAnimationNode &
jleAnimation::getRootNode()
{
    return _rootNode;
}
std::unordered_map<std::string, jleSkinnedMeshBone> &
jleAnimation::getBoneMapping()
{
    return _boneMapping;
}
void
jleAnimation::readBones(const aiAnimation *animation)
{
    int numChannels = animation->mNumChannels;
    for (int i = 0; i < numChannels; i++)
    {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        if (_boneMapping.find(boneName) == _boneMapping.end())
        {
            int newIndex = _boneMapping.size();
            _boneMapping[boneName].index = newIndex;
        }
        _bones.push_back({channel->mNodeName.data, _boneMapping[channel->mNodeName.data].index, channel});
    }
}
void
jleAnimation::readHierarchyData(jleAnimationNode &dest, aiNode *src)
{
    assert(src);

    dest.name = src->mName.C_Str();
    dest.transformation = glm::transpose(glm::make_mat4(&src->mTransformation.a1));

    int children = src->mNumChildren;
    for(int i = 0; i < children; ++i){
        jleAnimationNode newNode;
        readHierarchyData(newNode, src->mChildren[i]);
        dest.childNodes.push_back(newNode);
    }
}
std::vector<std::string>
jleAnimation::getFileAssociationList()
{
    return {"fbx"};
}
