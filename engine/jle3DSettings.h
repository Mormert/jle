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

    jleResourceRef<jleSkybox> skybox;

    void
    setDirectionalLight(const glm::mat4 &view, const glm::vec3 &colour)
    {
        directionalLightRotation = glm::normalize(glm::vec3(view[2][0], view[2][1], view[2][2]));
        directionalLightColour = colour;
    }


    glm::mat4 getLightSpaceMatrixAtPosition(const glm::vec3& position) const
    {
        const static glm::mat4 lightProjection = glm::ortho(-160.0f, 160.0f, -160.0f, 160.0f, -500.f, 500.f);

        auto mat = lightProjection * glm::lookAt(position, position - directionalLightRotation, glm::vec3(0.0f, 1.0f, 0.0f));
        return mat;
    }

};
