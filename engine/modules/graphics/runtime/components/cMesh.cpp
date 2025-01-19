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

#include "cMesh.h"
#include <modules/hierarchy/components/cTransform.h>

void
cMesh::ecsUpdate(jleFramePacket &packet, const glm::mat4& worldMatrix, int instanceId)
{
    if (_meshRef) {
        std::shared_ptr<jleMesh> mesh = _meshRef.get();
        std::shared_ptr<jleMaterial> material = _materialRef.get();
        packet.sendMesh(mesh, material, worldMatrix, instanceId, true);
    }
}

std::shared_ptr<jleMesh>
cMesh::getMesh()
{
    return _meshRef.get();
}

std::shared_ptr<jleMaterial>
cMesh::getMaterial()
{
    return _materialRef.get();
}

jleResourceRef<jleMesh> &
cMesh::getMeshRef()
{
    return _meshRef;
}

jleResourceRef<jleMaterial> &
cMesh::getMaterialRef()
{
    return _materialRef;
}

