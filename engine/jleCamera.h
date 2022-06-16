// Copyright (c) 2022. Johan Lind

#pragma once

namespace jle {
    struct jleCamera {
        float mX = 0.f, mY = 0.f;

        [[nodiscard]] uint32_t GetIntX() const{
            return uint32_t(mX);
        }

        [[nodiscard]] uint32_t GetIntY() const{
            return uint32_t(mY);
        }
    };
}
