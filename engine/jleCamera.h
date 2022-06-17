// Copyright (c) 2022. Johan Lind

#pragma once

namespace jle {
    struct jleCamera {
        float mX = 0.f, mY = 0.f;

        [[nodiscard]] int32_t GetIntX() const{
            return int32_t(mX);
        }

        [[nodiscard]] int32_t GetIntY() const{
            return int32_t(mY);
        }
    };
}
