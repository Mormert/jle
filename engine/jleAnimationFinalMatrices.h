// Copyright (c) 2023. Johan Lind

#pragma once

#include <glm/glm.hpp>
#include <vector>

struct jleAnimationFinalMatrices{
    std::vector<glm::mat4> matrices;

    jleAnimationFinalMatrices() {
        matrices.reserve(100);
        for(int i = 0; i < 100; ++i){
            matrices.emplace_back(1.f);
        }
    }
};