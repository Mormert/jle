// Copyright (c) 2023. Johan Lind

#include "jleAnimation.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/gtc/type_ptr.hpp>

bool
jleAnimation::loadFromFile(const jlePath &path)
{
    Assimp::Importer importer;
    // importer.SetPropertyBool("AI_CONFIG_FBX_USE_SKELETON_BONE_CONTAINER", true);
    const aiScene *scene = importer.ReadFile(
        path.getRealPath(), aiProcess_Triangulate | aiProcess_PopulateArmatureData | aiProcess_LimitBoneWeights);
    if (!scene || !scene->mRootNode) {
        LOGE << "Failed loading animation, no scene or no root node.";
        return false;
    }

    if (!scene->HasAnimations()) {
        LOGE << "Failed loading animation, loaded model is missing animations.";
        return false;
    }

    if (!scene->HasMeshes()) {
        LOGE << "Failed loading animation, loaded model is missing at least 1 mesh to load skeleton";
        return false;
    }

    auto animation = scene->mAnimations[0];
    _duration = animation->mDuration;
    _ticksPerSecond = animation->mTicksPerSecond;
    readHierarchyData(_rootNode, scene->mRootNode);

    readBonesFromMesh(animation, scene->mMeshes[0]);

    return true;
}
jleAnimation::jleAnimation(const std::string &path, jleSkinnedMesh &mesh) {}

jleAnimationBone *
jleAnimation::findBone(const std::string &name)
{
    for (auto &i : _bones) {
        if (i.getName() == name) {
            return &i;
        }
    }
    return nullptr;
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
jleAnimation::readBonesFromMesh(const aiAnimation *animation, const aiMesh *mesh)
{
    // The way the FBX format seems to work, is that you need to have at least 1 mesh to save
    // bone/skeletal data. This seems to also be a necessity in UE4 when loading rigged characters from FBX files.

    int numChannels = animation->mNumChannels;

    const auto findBoneInMesh = [&](const std::string &boneName) -> aiBone * {
        for (int i = 0; i < mesh->mNumBones; i++) {
            if (mesh->mBones[i]->mName.C_Str() == boneName) {
                return mesh->mBones[i];
            }
        }
        return nullptr;
    };

    for (int i = 0; i < numChannels; i++) {
        auto channel = animation->mChannels[i];

        std::string boneName = channel->mNodeName.data;

        if (_boneMapping.find(boneName) == _boneMapping.end()) {
            int newIndex = _boneMapping.size();
            if (findBoneInMesh(boneName)) {
                _boneMapping[boneName].index = newIndex;
            }
        }

        auto bone = findBoneInMesh(boneName);
        if (bone) {
            _boneMapping[boneName].offset = glm::transpose(glm::make_mat4(&bone->mOffsetMatrix.a1));
            _bones.push_back({channel->mNodeName.C_Str(), _boneMapping[channel->mNodeName.C_Str()].index, channel});
        }
    }
}

void
jleAnimation::readHierarchyData(jleAnimationNode &dest, aiNode *src)
{
    assert(src);

    dest.name = src->mName.C_Str();
    dest.transformation = glm::transpose(glm::make_mat4(&src->mTransformation.a1));

    int children = src->mNumChildren;
    for (int i = 0; i < children; ++i) {
        jleAnimationNode newNode;
        readHierarchyData(newNode, src->mChildren[i]);
        dest.childNodes.push_back(newNode);
    }
}

const std::vector<jleAnimationBone> &
jleAnimation::getBones()
{
    return _bones;
}
