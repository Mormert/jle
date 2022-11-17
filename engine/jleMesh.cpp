// Copyright (c) 2022. Johan Lind

#include "jleMesh.h"
#include "plog/Log.h"
#include "tiny_obj_loader.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else

#include <glad/glad.h>

#endif

bool
jleMesh::loadFromFile(const std::string &path)
{
    return loadFromObj(path);
}

bool
jleMesh::loadFromObj(const std::string &path)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool loaded = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());

    if (!err.empty()) {
        LOGE << err;
        return false;
    }

    const auto &vertices = attrib.vertices;
    const auto &normals = attrib.normals;
    const auto &texcoords = attrib.texcoords;

    std::vector<unsigned int> vertexIndicies;
    for (auto &&vertexIndex : shapes[0].mesh.indices) {
        vertexIndicies.push_back(vertexIndex.vertex_index);
    }

    makeMesh(vertices, normals, texcoords, vertexIndicies);

    return true;
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
jleMesh::makeMesh(const std::vector<float> &positions,
                  const std::vector<float> &normals,
                  const std::vector<float> &texCoords,
                  const std::vector<unsigned int> &indicies)
{

    destroyOldBuffers();

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    if (!positions.empty()) {
        glGenBuffers(1, &_vbo_pos);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_pos);
        glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), &positions[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        glEnableVertexAttribArray(0);
    }

    if (!normals.empty()) {
        glGenBuffers(1, &_vbo_normal);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_normal);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        glEnableVertexAttribArray(1);
    }

    if (!texCoords.empty()) {
        glGenBuffers(1, &_vbo_texcoords);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_texcoords);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), &texCoords[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
        glEnableVertexAttribArray(2);
    }

    if (!indicies.empty()) {
        glGenBuffers(1, &_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(unsigned int), &indicies[0], GL_STATIC_DRAW);
    }

    glBindVertexArray(0);

    if (indicies.size() > 0) {
        _trianglesCount = indicies.size();
    } else {
        _trianglesCount = positions.size() / 3;
    }
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
    if (_ebo) {
        glDeleteBuffers(1, &_ebo);
        _ebo = 0;
    }
    if (_vao) {
        glDeleteVertexArrays(1, &_vao);
        _vao = 0;
    }
}
