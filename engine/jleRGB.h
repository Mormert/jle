// Copyright (c) 2023. Johan Lind

#pragma once

#include <glm/glm.hpp>

class jleRGB : public glm::vec3
{
public:
    jleRGB() : glm::vec3(1.f) {}
    jleRGB(float v) : glm::vec3(v) {}
    jleRGB(float x, float y, float z) : glm::vec3(x, y, z) {}
    jleRGB(const glm::vec3 &other) : glm::vec3(other) {}
    jleRGB &
    operator=(const glm::vec3 &other)
    {
        glm::vec3::operator=(other);
        return *this;
    }
};

class jleRGBA : public glm::vec4
{
public:
    jleRGBA() : glm::vec4(1.f) {}
    jleRGBA(float v) : glm::vec4(v) {}
    jleRGBA(float x, float y, float z, float w) : glm::vec4(x, y, z, w) {}
    jleRGBA(const glm::vec4 &other) : glm::vec4(other) {}
    jleRGBA &
    operator=(const glm::vec4 &other)
    {
        glm::vec4::operator=(other);
        return *this;
    }
};