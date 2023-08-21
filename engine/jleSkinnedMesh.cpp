// Copyright (c) 2023. Johan Lind


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

jleSkinnedMesh::~jleSkinnedMesh() {
    destroyOldBuffersSkinned();
}


void
jleSkinnedMesh::makeSkinnedMesh(const std::vector<glm::vec3> &positions,
                                const std::vector<glm::vec3> &normals,
                                const std::vector<glm::vec2> &texCoords,
                                const std::vector<glm::vec3> &tangents,
                                const std::vector<glm::vec3> &bitangents,
                                const std::vector<unsigned int> &indices,
                                const std::vector<glm::ivec4>& boneIndices,
                                const std::vector<glm::vec4>& boneWeights)
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

}

void
jleSkinnedMesh::loadAssimpSkinnedMesh(aiMesh *assimpMesh,
                                      std::vector<glm::vec3> &out_positions,
                                      std::vector<glm::vec3> &out_normals,
                                      std::vector<glm::vec2> &out_texCoords,
                                      std::vector<glm::vec3> &out_tangents,
                                      std::vector<glm::vec3> &out_bitangents,
                                      std::vector<unsigned int> &out_indices,
                                      std::vector<glm::ivec4> out_boneIndices,
                                      std::vector<glm::vec4> out_boneWeights,
                                      std::unordered_map<std::string, jleSkinnedMeshBone>& out_boneMapping)
{
    loadAssimpMesh(assimpMesh, out_positions, out_normals, out_texCoords, out_tangents, out_bitangents, out_indices);

    if(assimpMesh->HasBones()){

        for(int i = 0; i < assimpMesh->mNumVertices; i++){
            glm::ivec4 boneIndex = glm::ivec4{-1,-1,-1,-1};
            glm::vec4 boneWeight = glm::vec4{0.f};

            out_boneIndices.push_back(boneIndex);
            out_boneWeights.push_back(boneWeight);
        }

        for(int boneIndex = 0; boneIndex < assimpMesh->mNumBones; ++boneIndex){
            int boneId = 0;
            std::string boneName = assimpMesh->mBones[boneIndex]->mName.C_Str();
            if(out_boneMapping.find(boneName) == out_boneMapping.end()){
                jleSkinnedMeshBone newBone;
                newBone.index = out_boneMapping.size();
                newBone.offset = glm::transpose(glm::make_mat4(&assimpMesh->mBones[boneIndex]->mOffsetMatrix.a1));
                boneId = (int)out_boneMapping.size();
                out_boneMapping[boneName] = newBone;
            }else{
                boneId = out_boneMapping[boneName].index;
            }
            assert(boneId != -1);

            auto weights = assimpMesh->mBones[boneIndex]->mWeights;
            const int numWeights = assimpMesh->mBones[boneIndex]->mNumWeights;

            for(int weightIndex = 0; weightIndex < numWeights; weightIndex++){
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                assert(vertexId <= out_positions.size());

                const auto setVertexBoneData = [&](int vertexId, int boneId, float weight){
                    for(int i = 0; i < 4; i++){
                        if(out_boneIndices[vertexId][i] < 0){
                            out_boneWeights[vertexId][i] = weight;
                            out_boneIndices[vertexId][i] = boneId;
                            break;
                        }
                    }
                };

                setVertexBoneData(vertexId, boneId, weight);
            }

        }


    }else{
        LOGE << "Failed loading skinned mesh since it doesn't have bones.";
        return;
    }
}
