// Copyright (c) 2023. Johan Lind

#include "jleAnimationFinalMatrices.h"

jleAnimationFinalMatrices::jleAnimationFinalMatrices()
{
    matrices.reserve(100);
    for(int i = 0; i < 100; ++i){
        matrices.emplace_back(1.f);
    }
}
