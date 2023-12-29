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

#include "jleSkinnedMesh.h"

#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/gtc/type_ptr.hpp>

#include "jleIncludeGL.h"

void
jleSkinnedMesh::destroyOldBuffersSkinned()
{
    destroyOldBuffers();

    if (_vbo_boneIndices) {
        glDeleteBuffers(1, &_vbo_boneIndices);
        _vbo_boneIndices = 0;
    }
    if (_vbo_boneWeights) {
        glDeleteBuffers(1, &_vbo_boneWeights);
        _vbo_boneWeights = 0;
    }
}

jleSkinnedMesh::~jleSkinnedMesh() { destroyOldBuffersSkinned(); }

void
jleSkinnedMesh::makeSkinnedMesh(const std::vector<glm::vec3> &positions,
                                const std::vector<glm::vec3> &normals,
                                const std::vector<glm::vec2> &texCoords,
                                const std::vector<glm::vec3> &tangents,
                                const std::vector<glm::vec3> &bitangents,
                                const std::vector<unsigned int> &indices,
                                const std::vector<glm::ivec4> &boneIndices,
                                const std::vector<glm::vec4> &boneWeights,
                                const std::unordered_map<std::string, jleSkinnedMeshBone> &boneMapping)
{
    destroyOldBuffersSkinned();
    makeMesh(positions, normals, texCoords, tangents, bitangents, indices);

    if (!boneIndices.empty()) {
        glGenBuffers(1, &_vbo_boneIndices);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_boneIndices);
        glBufferData(GL_ARRAY_BUFFER, boneIndices.size() * sizeof(glm::ivec4), &boneIndices[0], GL_STATIC_DRAW);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(float) * 4, 0);
        glEnableVertexAttribArray(5);
    }

    if (!boneWeights.empty()) {
        glGenBuffers(1, &_vbo_boneWeights);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_boneWeights);
        glBufferData(GL_ARRAY_BUFFER, boneWeights.size() * sizeof(glm::vec4), &boneWeights[0], GL_STATIC_DRAW);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
        glEnableVertexAttribArray(6);
    }

    _boneIndices = boneIndices;
    _boneWeights = boneWeights;
    _boneMapping = boneMapping;
}

bool
jleSkinnedMesh::loadAssimpSkinnedMesh(aiMesh *assimpMesh,
                                      std::vector<glm::vec3> &out_positions,
                                      std::vector<glm::vec3> &out_normals,
                                      std::vector<glm::vec2> &out_texCoords,
                                      std::vector<glm::vec3> &out_tangents,
                                      std::vector<glm::vec3> &out_bitangents,
                                      std::vector<unsigned int> &out_indices,
                                      std::vector<glm::ivec4> &out_boneIndices,
                                      std::vector<glm::vec4> &out_boneWeights,
                                      std::unordered_map<std::string, jleSkinnedMeshBone> &out_boneMapping)
{
    loadAssimpMesh(assimpMesh, out_positions, out_normals, out_texCoords, out_tangents, out_bitangents, out_indices);

    if (assimpMesh->HasBones()) {

        for (int i = 0; i < assimpMesh->mNumVertices; i++) {
            glm::ivec4 boneIndex = glm::ivec4{-1, -1, -1, -1};
            glm::vec4 boneWeight = glm::vec4{0.f};

            out_boneIndices.push_back(boneIndex);
            out_boneWeights.push_back(boneWeight);
        }

        for (int boneIndex = 0; boneIndex < assimpMesh->mNumBones; ++boneIndex) {
            int boneId = 0;
            std::string boneName = assimpMesh->mBones[boneIndex]->mName.C_Str();
            if (out_boneMapping.find(boneName) == out_boneMapping.end()) {
                jleSkinnedMeshBone newBone;
                newBone.index = out_boneMapping.size();
                newBone.offset = glm::transpose(glm::make_mat4(&assimpMesh->mBones[boneIndex]->mOffsetMatrix.a1));
                boneId = (int)out_boneMapping.size();
                out_boneMapping[boneName] = newBone;
            } else {
                boneId = out_boneMapping[boneName].index;
            }
            assert(boneId != -1);

            auto weights = assimpMesh->mBones[boneIndex]->mWeights;
            const int numWeights = assimpMesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; weightIndex++) {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                assert(vertexId <= out_positions.size());

                const auto setVertexBoneData = [&](int vertexId, int boneId, float weight) {
                    for (int i = 0; i < 4; i++) {
                        if (out_boneIndices[vertexId][i] < 0) {
                            out_boneWeights[vertexId][i] = weight;
                            out_boneIndices[vertexId][i] = boneId;
                            break;
                        }
                    }
                };

                setVertexBoneData(vertexId, boneId, weight);
            }
        }

        return true;
    } else {
        LOGE << "Failed loading skinned mesh since it doesn't have bones.";
        return false;
    }
}

bool
jleSkinnedMesh::loadFromFile(const jlePath &path)
{
    bool ret = loadSkinnedAssimp(path);
    if (!ret) {
        return false;
    }
    return true;
}

bool
jleSkinnedMesh::loadSkinnedAssimp(const jlePath &path)
{
    auto pathStr = path.getRealPath();

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(pathStr,
                                             aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_GenNormals |
                                                 aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_LimitBoneWeights);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOGE << "Error loading mesh with Assimp" << importer.GetErrorString();
        return false;
    }

    std::vector<glm::vec3> out_vertices;
    std::vector<glm::vec2> out_uvs;
    std::vector<glm::vec3> out_normals;
    std::vector<glm::vec3> out_tangents;
    std::vector<glm::vec3> out_bitangents;
    std::vector<unsigned int> out_indices;
    std::vector<glm::ivec4> out_boneIndices;
    std::vector<glm::vec4> out_boneWeights;
    std::unordered_map<std::string, jleSkinnedMeshBone> out_boneMapping;

    if (scene->mNumMeshes >= 1) {
        auto assimpMesh = scene->mMeshes[0];
        loadAssimpSkinnedMesh(assimpMesh,
                              out_vertices,
                              out_normals,
                              out_uvs,
                              out_tangents,
                              out_bitangents,
                              out_indices,
                              out_boneIndices,
                              out_boneWeights,
                              out_boneMapping);
    } else {
        LOGE << "Found no skinned meshes in " << path.getVirtualPath();
        return false;
    }

    if (scene->mNumMeshes > 1) {
        LOGW << "Found multiple skinned meshes in " << path.getVirtualPath() << ", only first mesh found will be used!";
    }

    makeSkinnedMesh(out_vertices,
                    out_normals,
                    out_uvs,
                    out_tangents,
                    out_bitangents,
                    out_indices,
                    out_boneIndices,
                    out_boneWeights,
                    out_boneMapping);

    processBoneNode(scene, scene->mRootNode, boneHierarchy);

    LOGV << "Loaded skinned mesh " << path.getVirtualPath() << " with " << out_vertices.size() << " vertices";

    return true;
}

void
jleSkinnedMesh::saveToFile()
{
    aiScene scene;
    jleMesh::saveMeshToAssimpScene(scene);

    auto assimpMesh = scene.mMeshes[0];

    assimpMesh->mBones = new aiBone *[_boneMapping.size()]();
    assimpMesh->mNumBones = _boneMapping.size();

    fillNode(scene.mRootNode, boneHierarchy);

    int b = 0;
    for (const auto &bone : _boneMapping) {
        auto assimpBone = assimpMesh->mBones[b] = new aiBone();

        auto relatedBone = scene.mRootNode->FindNode(bone.first.c_str());

        if (relatedBone) {
            assimpBone->mNode = relatedBone;
            assimpBone->mName.Set(relatedBone->mName.C_Str());
        }

        int index = bone.second.index;

        struct IndexWeight {
            int index;
            float weight;
        };

        std::vector<IndexWeight> indexWeights;

        for (int j = 0; j < _boneIndices.size(); ++j) {
            for (int k = 0; k < 4; ++k) {
                if (_boneIndices[j][k] == index) {
                    indexWeights.push_back({j, _boneWeights[j][k]});
                }
            }
        }

        assimpBone->mNumWeights = indexWeights.size();
        assimpBone->mWeights = new aiVertexWeight[indexWeights.size()];

        for (int j = 0; j < indexWeights.size(); ++j) {
            assimpBone->mWeights[j].mWeight = indexWeights[j].weight;
            assimpBone->mWeights[j].mVertexId = indexWeights[j].index;
        }

        b++;
    }

    Assimp::Exporter exporter;
    const auto &format = path.getFileEnding();
    auto ret = exporter.Export(&scene,
                               format,
                               path.getRealPath(),
                               aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs);

    if (ret == aiReturn_SUCCESS) {
        LOGI << "Exported skinned mesh " << path.getVirtualPath() << " successfully.";
    } else {
        LOGE << "Failed to save skinned mesh: " << path.getVirtualPath();
    }
}

void
jleSkinnedMesh::processBoneNode(const aiScene *scene, const aiNode *boneNode, jleSkinnedMeshBoneHierarchy &hierarchy)
{
    auto bone = _boneMapping.find(boneNode->mName.C_Str());
    if (bone != _boneMapping.end()) {
        hierarchy.offset = bone->second.offset;
    }

    hierarchy.name = boneNode->mName.C_Str();
    hierarchy.transformation = glm::transpose(glm::make_mat4(&boneNode->mTransformation.a1));

    const auto isNodeABone = [&](const aiNode *node) {
        for (int i = 0; i < scene->mMeshes[0]->mNumBones; i++) {
            if (scene->mMeshes[0]->mBones[i]->mName == node->mName) {
                return true;
            }
        }
        return false;
    };

    int k = 0;
    for (int i = 0; i < boneNode->mNumChildren; i++) {
        // if (isNodeABone(boneNode->mChildren[i]) || boneNode->mChildren[i]->mName == scene->mRootNode->mName) {
        hierarchy.children.emplace_back();
        processBoneNode(scene, boneNode->mChildren[i], hierarchy.children[k++]);
        //}
    }
}

void
jleSkinnedMesh::fillNode(aiNode *node, const jleSkinnedMesh::jleSkinnedMeshBoneHierarchy &hierarchy)
{
    auto nodeArray = new aiNode *[hierarchy.children.size()]();
    for (int i = 0; i < hierarchy.children.size(); i++) {
        nodeArray[i] = new aiNode();
        fillNode(nodeArray[i], hierarchy.children[i]);
    }

    node->addChildren(hierarchy.children.size(), nodeArray);

    node->mName.Set(hierarchy.name);

    glm::mat4 transformTranspose = glm::transpose(hierarchy.transformation);
    ::memcpy(&node->mTransformation, &transformTranspose, sizeof(aiMatrix4x4));
}

std::unordered_map<std::string, jleSkinnedMeshBone> &
jleSkinnedMesh::getBoneMapping()
{
    return _boneMapping;
}
