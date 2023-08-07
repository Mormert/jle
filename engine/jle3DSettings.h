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

        directionalLightRotation = glm::normalize(glm::vec3(view[0][2], view[1][2], view[2][2]));
        directionalLightColour = colour;

        const static glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -500.f, 500.f);

        lightSpaceMatrix = lightProjection * view;
    }
};
