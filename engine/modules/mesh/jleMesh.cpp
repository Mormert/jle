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

#include "jleMesh.h"

#include <core/serialization/jleSerialization.h>


#include <plog/Log.h>
#include <tinyobjloader/tiny_obj_loader.h>

#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>



bool
jleMesh::loadFromFile(jleSerializationContext &ctx, const jlePath &path)
{
    bool ret = loadAssimp(path);
    if (ret) {
        return true;
    } else {
        return false;
    }
}


unsigned int
jleMesh::getTrianglesCount() const
{
    return _trianglesCount;
}

void
jleMesh::makeMesh(const std::vector<glm::vec3> &positions,
                  const std::vector<glm::vec3> &normals,
                  const std::vector<glm::vec2> &texCoords,
                  const std::vector<glm::vec3> &tangents,
                  const std::vector<glm::vec3> &bitangents,
                  const std::vector<unsigned int> &indices)
{
    if (!indices.empty()) {
        _trianglesCount = indices.size();
    } else {
        _trianglesCount = positions.size();
    }

    _positions = positions;
    _normals = normals;
    _texCoords = texCoords;
    _tangents = tangents;
    _bitangents = bitangents;
    _indices = indices;
}

jleMesh::jleMesh() = default;

jleMesh::~jleMesh() = default;

bool
jleMesh::usesIndexing() const
{
    return !_indices.empty();
}

bool
jleMesh::loadAssimp(const jlePath &path)
{
    auto pathStr = path.getRealPath().str();

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(pathStr,
                                             aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_GenNormals |
                                                 aiProcess_CalcTangentSpace | aiProcess_FlipUVs);

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

    if (scene->mNumMeshes >= 1) {
        auto assimpMesh = scene->mMeshes[0];
        loadAssimpMesh(assimpMesh, out_vertices, out_normals, out_uvs, out_tangents, out_bitangents, out_indices);
    } else {
        LOGW << "Found no meshes in " << path.getVirtualPath().str();
        return false;
    }

    if (scene->mNumMeshes > 1) {
        LOGW << "Found multiple meshes in " << path.getVirtualPath().str() << ", only first mesh found will be used!";
    }

    makeMesh(out_vertices, out_normals, out_uvs, out_tangents, out_bitangents, out_indices);

    LOGV << "Loaded mesh " << path.getVirtualPath().str() << " with " << out_vertices.size() << " vertices";

    return true;
}

void
jleMesh::loadAssimpMesh(aiMesh *assimpMesh,
                        std::vector<glm::vec3> &out_positions,
                        std::vector<glm::vec3> &out_normals,
                        std::vector<glm::vec2> &out_texCoords,
                        std::vector<glm::vec3> &out_tangents,
                        std::vector<glm::vec3> &out_bitangents,
                        std::vector<unsigned int> &out_indices)
{
    for (int j = 0; j < assimpMesh->mNumVertices; j++) {

        glm::vec3 position;
        position.x = assimpMesh->mVertices[j].x;
        position.y = assimpMesh->mVertices[j].y;
        position.z = assimpMesh->mVertices[j].z;
        out_positions.push_back(position);

        if (assimpMesh->HasNormals()) {
            glm::vec3 normal;
            normal.x = assimpMesh->mNormals[j].x;
            normal.y = assimpMesh->mNormals[j].y;
            normal.z = assimpMesh->mNormals[j].z;
            out_normals.push_back(normal);
        }

        // Does the mesh have UVs?
        if (assimpMesh->mTextureCoords[0]) {
            glm::vec2 coords;

            coords.x = assimpMesh->mTextureCoords[0][j].x;
            coords.y = assimpMesh->mTextureCoords[0][j].y;
            out_texCoords.push_back(coords);

            glm::vec3 tangent;
            tangent.x = assimpMesh->mTangents[j].x;
            tangent.y = assimpMesh->mTangents[j].y;
            tangent.z = assimpMesh->mTangents[j].z;
            out_tangents.push_back(tangent);

            glm::vec3 bitangent;
            tangent.x = assimpMesh->mBitangents[j].x;
            tangent.y = assimpMesh->mBitangents[j].y;
            tangent.z = assimpMesh->mBitangents[j].z;
            out_bitangents.push_back(bitangent);
        } else {
            glm::vec2 defaultCoords(0.0f, 0.0f);
            out_texCoords.push_back(defaultCoords);

            glm::vec3 defaultTangent(1.0f, 0.0f, 0.0f);
            out_tangents.push_back(defaultTangent);

            glm::vec3 defaultBitangent(0.0f, 1.0f, 0.0f);
            out_bitangents.push_back(defaultBitangent);
        }
    }

    for (int i = 0; i < assimpMesh->mNumFaces; i++) {
        const auto &face = assimpMesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++) {
            out_indices.push_back(face.mIndices[j]);
        }
    }
}

const std::vector<glm::vec3> &
jleMesh::positions() const
{
    return _positions;
}

const std::vector<glm::vec3> &
jleMesh::normals() const
{
    return _normals;
}

const std::vector<glm::vec2> &
jleMesh::texCoords() const
{
    return _texCoords;
}

const std::vector<glm::vec3> &
jleMesh::tangents() const
{
    return _tangents;
}

const std::vector<glm::vec3> &
jleMesh::bitangents() const
{
    return _bitangents;
}

const std::vector<unsigned int> &
jleMesh::indices() const
{
    return _indices;
}

void
jleMesh::saveToFile(jleSerializationContext &ctx)
{
    aiScene scene;
    saveMeshToAssimpScene(scene);

    Assimp::Exporter exporter;
    const auto &format = path.getFileEnding();
    auto ret = exporter.Export(&scene, format, path.getRealPath().str(), aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs);

    if (ret == aiReturn_SUCCESS) {
        LOGI << "Exported " << path.getVirtualPath().str() << " successfully.";
    } else {
        const char *errorString = exporter.GetErrorString();
        LOGE << "Failed to save mesh: " << path.getVirtualPath().str() << ", reason: " << errorString;
    }
}

void
jleMesh::saveMeshToAssimpScene(aiScene &scene)
{
    scene.mRootNode = new aiNode();

    scene.mMaterials = new aiMaterial *[1];
    scene.mMaterials[0] = nullptr;
    scene.mNumMaterials = 1;

    scene.mMaterials[0] = new aiMaterial();

    scene.mMeshes = new aiMesh *[1];
    scene.mMeshes[0] = nullptr;
    scene.mNumMeshes = 1;

    scene.mMeshes[0] = new aiMesh();
    scene.mMeshes[0]->mMaterialIndex = 0;

    scene.mRootNode->mMeshes = new unsigned int[1];
    scene.mRootNode->mMeshes[0] = 0;
    scene.mRootNode->mNumMeshes = 1;

    auto assimpMesh = scene.mMeshes[0];

    assimpMesh->mVertices = new aiVector3D[_positions.size()];
    assimpMesh->mNumVertices = _positions.size();

    assimpMesh->mTextureCoords[0] = new aiVector3D[_texCoords.size()];
    assimpMesh->mNumUVComponents[0] = 2;

    assimpMesh->mNormals = new aiVector3D[_normals.size()];
    assimpMesh->mTangents = new aiVector3D[_tangents.size()];
    assimpMesh->mBitangents = new aiVector3D[_bitangents.size()];

    for (int i = 0; i < _positions.size(); i++) {
        assimpMesh->mVertices[i] = aiVector3D(_positions[i].x, _positions[i].y, _positions[i].z);
        assimpMesh->mTextureCoords[0][i] = aiVector3D(_texCoords[i].x, _texCoords[i].y, 0.f);
        assimpMesh->mNormals[i] = aiVector3D(_normals[i].x, _normals[i].y, _normals[i].z);
        assimpMesh->mTangents[i] = aiVector3D(_tangents[i].x, _tangents[i].y, _tangents[i].z);
        assimpMesh->mBitangents[i] = aiVector3D(_bitangents[i].x, _bitangents[i].y, _bitangents[i].z);
    }

    assimpMesh->mFaces = new aiFace[_indices.size() / 3];
    assimpMesh->mNumFaces = _indices.size() / 3;

    for (int i = 0; i < _indices.size() / 3; i++) {
        aiFace &face = assimpMesh->mFaces[i];

        face.mIndices = new unsigned int[3];
        face.mNumIndices = 3;

        face.mIndices[0] = _indices[3 * i + 0];
        face.mIndices[1] = _indices[3 * i + 1];
        face.mIndices[2] = _indices[3 * i + 2];
    }
}

