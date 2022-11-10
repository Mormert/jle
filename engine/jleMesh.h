// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFileLoadInterface.h"
#include <glm/glm.hpp>

class jleMesh : public jleFileLoadInterface
{
public:
    bool loadFromFile(const std::string &path) override;

    unsigned int getVAO();

    unsigned int getTrianglesCount();

private:
    // Synchronous OBJ loading
    bool loadObj(const std::string &path);

    struct jleMeshTriangle {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    unsigned int _trianglesCount;

    unsigned int _vao{};
    unsigned int _vbo_pos{};
    unsigned int _vbo_normal{};
    unsigned int _vbo_uv{};
    unsigned int _ebo{};
};
