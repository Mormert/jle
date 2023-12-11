// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleAnimationBone.h"
#include "jleSkinnedMesh.h"

#include <string>

#include <assimp/scene.h>
#include <glm/glm.hpp>

struct jleAnimationNode {
    glm::mat4 transformation;
    std::string name;
    std::vector<jleAnimationNode> childNodes;
};

class jleAnimation : public jleResourceInterface
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleAnimation, "anim", "anim.fbx")

    jleAnimation() = default;

    [[nodiscard]] bool loadFromFile(const jlePath &path) override;

    jleAnimation(const std::string &path, jleSkinnedMesh &mesh);

    jleAnimationBone *findBone(const std::string &name);

    float getTicksPerSec();

    float getDuration();

    jleAnimationNode &getRootNode();

    std::unordered_map<std::string, jleSkinnedMeshBone> &getBoneMapping();

    const std::vector<jleAnimationBone> &getBones();

private:
    void readBonesFromMesh(const aiAnimation *animation, const aiMesh *mesh);

    void readHierarchyData(jleAnimationNode &dest, aiNode *src);

    float _duration;
    int _ticksPerSecond;
    std::vector<jleAnimationBone> _bones;
    jleAnimationNode _rootNode;
    std::unordered_map<std::string, jleSkinnedMeshBone> _boneMapping;
};
