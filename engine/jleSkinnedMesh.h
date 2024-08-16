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

#include "jleMesh.h"

struct jleSkinnedMeshBone {
    int index{0};
    glm::mat4 offset{1.f};
};

struct aiNode;

class jleSkinnedMesh : public jleMesh
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleSkinnedMesh, "smesh", "smesh.fbx")

    [[nodiscard]] bool loadFromFile(const jlePath &path) override;

    ~jleSkinnedMesh() override;

    bool loadSkinnedAssimp(const jlePath &path);

    static bool loadAssimpSkinnedMesh(aiMesh *assimpMesh,
                                      std::vector<glm::vec3> &out_positions,
                                      std::vector<glm::vec3> &out_normals,
                                      std::vector<glm::vec2> &out_texCoords,
                                      std::vector<glm::vec3> &out_tangents,
                                      std::vector<glm::vec3> &out_bitangents,
                                      std::vector<unsigned int> &out_indices,
                                      std::vector<glm::ivec4> &out_boneIndices,
                                      std::vector<glm::vec4> &out_boneWeights,
                                      std::unordered_map<std::string, jleSkinnedMeshBone> &out_boneMapping);

    void makeSkinnedMesh(const std::vector<glm::vec3> &positions,
                         const std::vector<glm::vec3> &normals,
                         const std::vector<glm::vec2> &texCoords,
                         const std::vector<glm::vec3> &tangents,
                         const std::vector<glm::vec3> &bitangents,
                         const std::vector<unsigned int> &indices,
                         const std::vector<glm::ivec4> &boneIndices,
                         const std::vector<glm::vec4> &boneWeights,
                         const std::unordered_map<std::string, jleSkinnedMeshBone> &boneMapping);

    void saveToFile() override;

    std::unordered_map<std::string, jleSkinnedMeshBone> &
    getBoneMapping();

    struct jleSkinnedMeshBoneHierarchy {
        std::string name;
        glm::mat4 offset{1.f};
        glm::mat4 transformation{1.f};
        std::vector<jleSkinnedMeshBoneHierarchy> children;
    };

    jleSkinnedMeshBoneHierarchy boneHierarchy{};
    void processBoneNode(const aiScene *scene, const aiNode *boneNode, jleSkinnedMeshBoneHierarchy &hierarchy);

private:
    void destroyOldBuffersSkinned();

    unsigned int _vbo_boneIndices{};
    unsigned int _vbo_boneWeights{};

    std::vector<glm::ivec4> _boneIndices{};
    std::vector<glm::vec4> _boneWeights;

    std::unordered_map<std::string, jleSkinnedMeshBone> _boneMapping;

    void fillNode(aiNode *node, const jleSkinnedMeshBoneHierarchy &hierarchy);
};
