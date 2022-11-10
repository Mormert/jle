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
    return loadObj(path);
}

bool
jleMesh::loadObj(const std::string &path)
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

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glGenBuffers(1, &_vbo_pos);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_pos);
    glBufferData(GL_ARRAY_BUFFER, attrib.vertices.size() * sizeof(float), &attrib.vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &_vbo_normal);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_normal);
    glBufferData(GL_ARRAY_BUFFER, attrib.normals.size() * sizeof(float), &attrib.normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    glEnableVertexAttribArray(1);

    if (!attrib.texcoords.empty()) {
        glGenBuffers(1, &_vbo_uv);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_uv);
        glBufferData(GL_ARRAY_BUFFER, attrib.texcoords.size() * sizeof(float), &attrib.texcoords[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
        glEnableVertexAttribArray(2);
    }

    std::vector<int> vertexIndicies;
    for (auto &&vertexIndex : shapes[0].mesh.indices) {
        vertexIndicies.push_back(vertexIndex.vertex_index);
    }

    glGenBuffers(1, &_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, vertexIndicies.size() * sizeof(unsigned int), &vertexIndicies[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    _trianglesCount = shapes[0].mesh.indices.size();

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
