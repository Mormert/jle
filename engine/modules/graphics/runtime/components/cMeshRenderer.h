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

#include "modules/graphics/jleMaterial.h"
#include "core/jlePath.h"

class cTransform;
struct jleMeshGPUData;

class cMeshRenderer
{
public:
    template <class Archive>
    void
    serialize(Archive &ar){
        ar(CEREAL_NVP(_materialRef));
    }

    void ecsUpdate(jleFramePacket &packet, const glm::mat4& worldMatrix, int instanceId, const jlePath& meshPath, std::shared_ptr<jleMesh> mesh);

    std::shared_ptr<jleMaterial> getMaterial();     // Will be removed
    jleResourceRef<jleMaterial> &getMaterialRef();  // Will be removed

    [[nodiscard]] jlePath getGpuMeshPath() const { return _gpuMeshPath; }

    inline void setGpuMesh(const jlePath& path, jleMeshGPUDataHandle gpuMeshId) { _gpuMeshPath = path; _gpuMeshId = gpuMeshId; }

protected:
    // TODO: Replace with jlePath(Ref?)
    jleResourceRef<jleMaterial> _materialRef;

    jlePath _gpuMeshPath{};
    //jlePath _gpuMaterialPath{};

    jleMeshGPUDataHandle _gpuMeshId{jleMeshGPUDataHandle::InvalidValue};
    //uint32_t _gpuMaterialId = 0;
};