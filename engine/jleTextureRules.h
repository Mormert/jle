/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#pragma once

#define JLE_TEXTURE_DIR_SHADOW 0x84C0
#define JLE_TEXTURE_POINT_SHADOW 0x84C1
#define JLE_TEXTURE_ALBEDO 0x84C2
#define JLE_TEXTURE_NORMAL 0x84C3
#define JLE_TEXTURE_SKYBOX 0x84C4
#define JLE_TEXTURE_METALLIC 0x84C5
#define JLE_TEXTURE_ROUGHNESS 0x84C6
#define JLE_TEXTURE_OPACITY 0x84C7
#define JLE_TEXTURE_EMISSION 0x84C8

enum class jleTextureSlot : int {
    DirectionalShadow,
    PointShadow,
    Albedo,
    Normal,
    Skybox,
    Metallic,
    Roughness,
    Opacity,
    Emission
};