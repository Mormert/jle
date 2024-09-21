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

#include "jleFramePacket.h"
#include "jleMaterial.h"

void
jleFramePacket::sendMesh(std::shared_ptr<jleMesh> &mesh,
                     std::shared_ptr<jleMaterial> &material,
                     const glm::mat4 &transform,
                     int instanceId,
                     bool castShadows)
{
    if (material && material->isTranslucent()) {
        _translucentMeshes.emplace_back(jle3DQueuedMesh{mesh, material, transform, instanceId, castShadows});
    } else {
        _meshes.emplace_back(jle3DQueuedMesh{mesh, material, transform, instanceId, castShadows});
    }
}

void
jleFramePacket::sendSkinnedMesh(std::shared_ptr<jleSkinnedMesh> &mesh,
                            std::shared_ptr<jleMaterial> &material,
                            std::shared_ptr<jleAnimationFinalMatrices> &matrices,
                            const glm::mat4 &transform,
                            int instanceId,
                            bool castShadows)
{
    _skinnedMeshes.emplace_back(jle3DQueuedSkinnedMesh{mesh, material, matrices, transform, instanceId, castShadows});
}

void
jleFramePacket::sendLineStrip(const std::vector<jle3DLineVertex> &lines)
{
    _lineStrips.emplace_back(lines);
}

void
jleFramePacket::sendLines(const std::vector<jle3DLineVertex> &lines)
{
    _lines.insert(std::end(_lines), std::begin(lines), std::end(lines));
}

void
jleFramePacket::sendLine(const jle3DLineVertex &from, const jle3DLineVertex &to)
{
    _lines.emplace_back(from);
    _lines.emplace_back(to);
}

void
jleFramePacket::sendLight(const glm::vec3 &position, const glm::vec3 &color)
{
    _lights.emplace_back(jle3DRendererLight{position, color});
}

void
jleFramePacket::sendLine(const glm::vec3 &from, const glm::vec3 &to)
{
    _lines.emplace_back(jle3DLineVertex{from, glm::vec3{1.f}, glm::vec3{1.f, 0.f, 0.f}});
    _lines.emplace_back(jle3DLineVertex{to, glm::vec3{1.f}, glm::vec3{1.f, 0.f, 0.f}});
}
