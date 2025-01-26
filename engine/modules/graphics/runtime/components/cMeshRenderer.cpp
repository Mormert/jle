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

#include "cMeshRenderer.h"
#include <modules/hierarchy/components/cTransform.h>

void
cMeshRenderer::ecsUpdate(jleFramePacket &packet, const glm::mat4& worldMatrix, int instanceId, const jlePath& meshPath, std::shared_ptr<jleMesh> mesh)
{
    if (_gpuMeshId) {
        std::shared_ptr<jleMaterial> material = _materialRef.get();
        packet.sendMesh(_gpuMeshId, material, worldMatrix, instanceId, true);
    }
}

std::shared_ptr<jleMaterial>
cMeshRenderer::getMaterial()
{
    return _materialRef.get();
}

jleResourceRef<jleMaterial> &
cMeshRenderer::getMaterialRef()
{
    return _materialRef;
}