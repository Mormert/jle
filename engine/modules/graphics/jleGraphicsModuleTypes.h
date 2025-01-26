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

#include <cstdint>
#include <limits>
#include <functional>

template <typename T = uint32_t, typename Tag = void>
struct jleGpuHandle
{
    using ValueType = T;

    T value{InvalidValue};

    static constexpr T InvalidValue = std::numeric_limits<T>::max();

    constexpr jleGpuHandle() noexcept = default;
    constexpr explicit jleGpuHandle(T v) noexcept : value(v) {}

    constexpr operator T() const noexcept { return value; }

    constexpr explicit operator bool() const noexcept { return value != InvalidValue; }
};

namespace std {
template<typename T, typename Tag>
struct hash<jleGpuHandle<T, Tag>>
{
    constexpr size_t operator()(const jleGpuHandle<T, Tag>& h) const noexcept {
        return std::hash<T>{}(h.value);
    }
};
}

struct jleMeshGPUData {
    uint32_t vao = UINT32_MAX;
    uint32_t vbo_pos = UINT32_MAX;
    uint32_t vbo_normal = UINT32_MAX;
    uint32_t vbo_texcoords = UINT32_MAX;
    uint32_t vbo_tangent = UINT32_MAX;
    uint32_t vbo_bitangent = UINT32_MAX;
    uint32_t ebo = UINT32_MAX;

    uint32_t trianglesCount = 0;
    bool usesIndexing = false;
};

struct jleSkinnedMeshGPUData : public jleMeshGPUData {
    uint32_t vbo_boneIndices = UINT32_MAX;
    uint32_t vbo_boneWeights = UINT32_MAX;
};

using jleMeshGPUDataHandle = jleGpuHandle<uint32_t, struct jleMeshGPUDataTag>;
using jleSkinnedMeshGPUDataHandle = jleGpuHandle<uint32_t, struct jleSkinnedMeshGPUDataTag>;
