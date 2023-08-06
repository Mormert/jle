// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleResourceRef.h"
#include "jleSkybox.h"
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

struct jle3DSettings {
    bool useDirectionalLight{true};
    bool usePointsLights{true};
    bool useEnvironmentMapping{true};

    glm::vec3 directionalLightRotation{};
    glm::vec3 directionalLightColour{};
    glm::mat4 lightSpaceMatrix;

    jleResourceRef<jleSkybox> skybox;

    void
    setDirectionalLight(const glm::mat4 &view, const glm::vec3 &colour)
    {

        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(view, scale, rotation, translation, skew, perspective);

        directionalLightRotation = rotation * glm::vec3{0.f, 0.f, 1.f};
        directionalLightColour = colour;

        glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, -500.f, 500.f);
        glm::mat4 lightView =
            glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), rotation * glm::vec3{0.f, -1.f, 0.f}, glm::vec3(0.0f, -1.0f, 0.0f));
        lightView[1][1] *= -1.f;

        lightSpaceMatrix = lightProjection * view;
    }
};
