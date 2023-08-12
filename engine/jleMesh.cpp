// Copyright (c) 2023. Johan Lind

#include "jleMesh.h"
#include "tiny_obj_loader.h"
#include <plog/Log.h>
#include <stdio.h>

#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "jleIncludeGL.h"

jleLoadFromFileSuccessCode
jleMesh::loadFromFile(const jlePath &path)
{
    bool ret = loadFromObj(path);
    if (ret) {
        return jleLoadFromFileSuccessCode::SUCCESS;
    } else {
        return jleLoadFromFileSuccessCode::FAIL;
    }
}

bool
jleMesh::loadFromObj(const jlePath &path)
{

    return loadAssimp(path);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    const std::string materialPath = path.getRealPath().substr(0, path.getRealPath().find_last_of('/'));
    bool loaded =
        tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.getRealPath().c_str(), materialPath.c_str());

    if (!err.empty()) {
        LOGE << err;
        return false;
    }

    const auto &vertices = attrib.vertices;
    const auto &normals = attrib.normals;
    const auto &texcoords = attrib.texcoords;

    std::vector<glm::vec3> in_vertices;
    std::vector<glm::vec3> in_normals;
    std::vector<glm::vec2> in_uvs;

    for (int i = 0; i < vertices.size(); i += 3) {
        in_vertices.push_back({vertices[i], vertices[i + 1], vertices[i + 2]});
    }

    for (int i = 0; i < normals.size(); i += 3) {
        in_normals.push_back({normals[i], normals[i + 1], normals[i + 2]});
    }

    for (int i = 0; i < texcoords.size(); i += 2) {
        in_uvs.push_back({texcoords[i], texcoords[i + 1]});
    }

    // If the OBJ file uses indexing for all attributes, then we simply go thru the
    // mesh's indicies, and then find the attributes in the correct order, and then
    // create new attribute arrays. We thus, do not use indexed rendering.
    if (shapes[0].mesh.indices[0].normal_index >= 0) {
        std::vector<glm::vec3> out_vertices;
        std::vector<glm::vec2> out_uvs;
        std::vector<glm::vec3> out_normals;

        for (auto &&shape : shapes) {
            for (unsigned int i = 0; i < shape.mesh.indices.size(); i++) {

                // Get the indices of its attributes
                int vertexIndex = shape.mesh.indices[i].vertex_index;
                if (vertexIndex != -1) {
                    glm::vec3 vertex = in_vertices[vertexIndex];
                    out_vertices.push_back(vertex);
                }

                int uvIndex = shape.mesh.indices[i].texcoord_index;
                if (uvIndex != -1) {
                    glm::vec2 uv = in_uvs[uvIndex];
                    out_uvs.push_back(uv);
                }

                int normalIndex = shape.mesh.indices[i].normal_index;
                if (normalIndex != -1) {
                    glm::vec3 normal = in_normals[normalIndex];
                    out_normals.push_back(normal);
                }
            }
        }

        std::vector<glm::vec3> out_tangents;
        std::vector<glm::vec3> out_bitangents;
        if (!out_uvs.empty()) {
            for (unsigned int i = 0; i < out_vertices.size(); i += 3) {
                // Shortcuts for vertices
                glm::vec3 &v0 = out_vertices[i + 0];
                glm::vec3 &v1 = out_vertices[i + 1];
                glm::vec3 &v2 = out_vertices[i + 2];

                glm::vec2 &uv0 = out_uvs[i + 0];
                glm::vec2 &uv1 = out_uvs[i + 1];
                glm::vec2 &uv2 = out_uvs[i + 2];

                glm::vec3 deltaPos1 = v1 - v0;
                glm::vec3 deltaPos2 = v2 - v0;

                glm::vec2 deltaUV1 = uv1 - uv0;
                glm::vec2 deltaUV2 = uv2 - uv0;

                float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
                glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
                glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

                out_tangents.push_back(tangent);
                out_tangents.push_back(tangent);
                out_tangents.push_back(tangent);
                out_bitangents.push_back(bitangent);
                out_bitangents.push_back(bitangent);
                out_bitangents.push_back(bitangent);
            }
        }

        makeMesh(out_vertices, out_normals, out_uvs, out_tangents, out_bitangents);
        return true;

    } else { // Else we can use the vertex indices as they are, and thus use indexed rendering
        std::vector<unsigned int> vertexIndicies;
        for (auto &&shape : shapes) {
            for (int i = 0; i < shape.mesh.indices.size(); i++) {
                vertexIndicies.push_back(shape.mesh.indices[i].vertex_index);
            }
        }

        makeMesh(in_vertices, in_normals, in_uvs, {}, {}, vertexIndicies);
        return true;
    }
}

unsigned int
jleMesh::getVAO()
{
    return _vao;
}

unsigned int
jleMesh::getTrianglesCount()
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

    destroyOldBuffers();

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    if (!positions.empty()) {
        glGenBuffers(1, &_vbo_pos);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_pos);
        glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), &positions[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        glEnableVertexAttribArray(0);
    }

    if (!normals.empty()) {
        glGenBuffers(1, &_vbo_normal);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_normal);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        glEnableVertexAttribArray(1);
    }

    if (!texCoords.empty()) {
        glGenBuffers(1, &_vbo_texcoords);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_texcoords);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), &texCoords[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
        glEnableVertexAttribArray(2);
    }

    if (!tangents.empty()) {
        glGenBuffers(1, &_vbo_tangent);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_tangent);
        glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        glEnableVertexAttribArray(3);
    }

    if (!bitangents.empty()) {
        glGenBuffers(1, &_vbo_bitangent);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_bitangent);
        glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        glEnableVertexAttribArray(4);
    }

    if (!indices.empty()) {
        glGenBuffers(1, &_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    }

    glBindVertexArray(0);

    if (indices.size() > 0) {
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

jleMesh::~jleMesh() { destroyOldBuffers(); }

void
jleMesh::destroyOldBuffers()
{
    if (_vbo_pos) {
        glDeleteBuffers(1, &_vbo_pos);
        _vbo_pos = 0;
    }
    if (_vbo_normal) {
        glDeleteBuffers(1, &_vbo_normal);
        _vbo_normal = 0;
    }
    if (_vbo_texcoords) {
        glDeleteBuffers(1, &_vbo_texcoords);
        _vbo_texcoords = 0;
    }
    if (_vbo_tangent) {
        glDeleteBuffers(1, &_vbo_tangent);
        _vbo_tangent = 0;
    }
    if (_vbo_bitangent) {
        glDeleteBuffers(1, &_vbo_bitangent);
        _vbo_bitangent = 0;
    }
    if (_ebo) {
        glDeleteBuffers(1, &_ebo);
        _ebo = 0;
    }
    if (_vao) {
        glDeleteVertexArrays(1, &_vao);
        _vao = 0;
    }
}

bool
jleMesh::usesIndexing()
{
    return _ebo > 0;
}

bool
jleMesh::loadAssimp(const jlePath &path)
{
    auto pathStr = path.getRealPath();

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

    for (int i = 0; i < scene->mNumMeshes; i++) {
        auto assimpMesh = scene->mMeshes[i];
        loadAssimpMesh(assimpMesh, out_vertices, out_normals, out_uvs, out_tangents, out_bitangents, out_indices);
    }

    makeMesh(out_vertices, out_normals, out_uvs, out_tangents, out_bitangents, out_indices);

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
jleMesh::positions()
{
    return _positions;
}

const std::vector<glm::vec3> &
jleMesh::normals()
{
    return _normals;
}

const std::vector<glm::vec2> &
jleMesh::texCoords()
{
    return _texCoords;
}

const std::vector<glm::vec3> &
jleMesh::tangents()
{
    return _tangents;
}

const std::vector<glm::vec3> &
jleMesh::bitangents()
{
    return _bitangents;
}

const std::vector<unsigned int> &
jleMesh::indices()
{
    return _indices;
}

std::vector<std::string>
jleMesh::getFileAssociationList()
{
    return {"fbx", "obj"};
}

void
jleMesh::saveToFile()
{
    aiScene scene;

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

    Assimp::Exporter exporter;
    const auto &format = path.getFileEnding();
    auto ret = exporter.Export(&scene, format, path.getRealPath(), aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs);

    if (ret == aiReturn_SUCCESS) {
        LOGI << "Exported " << path.getVirtualPath() << " successfully.";
    } else {
        LOGE << "Failed to save mesh: " << path.getVirtualPath();
    }
}
