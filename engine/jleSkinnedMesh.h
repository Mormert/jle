// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleMesh.h"

struct jleSkinnedMeshBone{
    int index;
    glm::mat4 offset;
};

class jleSkinnedMesh : public jleMesh
{
public:

    jleLoadFromFileSuccessCode loadFromFile(const jlePath &path) override;

    ~jleSkinnedMesh() override;

    static void loadAssimpSkinnedMesh(aiMesh *assimpMesh,
                               std::vector<glm::vec3> &out_positions,
                               std::vector<glm::vec3> &out_normals,
                               std::vector<glm::vec2> &out_texCoords,
                               std::vector<glm::vec3> &out_tangents,
                               std::vector<glm::vec3> &out_bitangents,
                               std::vector<unsigned int> &out_indices,
                               std::vector<glm::ivec4> out_boneIndices,
                               std::vector<glm::vec4> out_boneWeights,
                               std::unordered_map<std::string, jleSkinnedMeshBone>& out_boneMapping);

    void makeSkinnedMesh(const std::vector<glm::vec3> &positions,
                         const std::vector<glm::vec3> &normals,
                         const std::vector<glm::vec2> &texCoords,
                         const std::vector<glm::vec3> &tangents,
                         const std::vector<glm::vec3> &bitangents,
                         const std::vector<unsigned int> &indices,
                         const std::vector<glm::ivec4>& boneIndices,
                         const std::vector<glm::vec4>& boneWeights);

private:

    void destroyOldBuffersSkinned();

    unsigned int _vbo_boneIndices{};
    unsigned int _vbo_boneWeights{};

    std::vector<glm::ivec4> _boneIndices{};
    std::vector<glm::vec4> _boneWeights;

    std::unordered_map<std::string, jleSkinnedMeshBone> _boneMapping;

};
