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

#include "core/jleCommon.h"

#include "core/jlePath.h"
#include "modules/graphics/jleGraphicsModuleTypes.h"

#include <glm/fwd.hpp>

class cTransform;
struct jleMeshGPUData;
class jleFramePacket;
class jleMesh;

class cMeshRenderer
{
public:
    template <class Archive>
    void
    serialize(Archive &ar){
        ar(CEREAL_NVP(_materialPath));
    }

    void ecsUpdate(jleFramePacket &packet, const glm::mat4& worldMatrix, int instanceId, const jlePath& meshPath, std::shared_ptr<jleMesh> mesh);

    [[nodiscard]] jlePath getMaterialPath() const { return _materialPath; }
    [[nodiscard]] jlePath getGpuMeshPath() const { return _gpuMeshPath; }
    [[nodiscard]] jlePath getGpuMaterialPath() const { return _gpuMaterialPath; }

    inline void setGpuMesh(const jlePath& path, jleMeshGPUDataHandle gpuMeshId) { _gpuMeshPath = path; _gpuMeshId = gpuMeshId; }
    inline void setGpuMaterial(const jlePath& path, jleMaterialGPUDataHandle gpuMaterialId) { _gpuMaterialPath = path; _gpuMaterialId = gpuMaterialId; }

protected:
    jlePath _materialPath{};

    jlePath _gpuMeshPath{};
    jlePath _gpuMaterialPath{};

    jleMeshGPUDataHandle _gpuMeshId{jleMeshGPUDataHandle::InvalidValue};
    jleMaterialGPUDataHandle _gpuMaterialId{jleMaterialGPUDataHandle::InvalidValue};
};