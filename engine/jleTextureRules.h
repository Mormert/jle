// Copyright (c) 2023. Johan Lind

#pragma once

#define JLE_TEXTURE_DIR_SHADOW 0x84C0
#define JLE_TEXTURE_POINT_SHADOW 0x84C1
#define JLE_TEXTURE_ALBEDO 0x84C2
#define JLE_TEXTURE_NORMAL 0x84C3
#define JLE_TEXTURE_SKYBOX 0x84C4

enum class jleTextureSlot : int {
    DirectionalShadow,
    PointShadow,
    Albedo,
    Normal,
    Skybox
};