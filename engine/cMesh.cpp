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
#include "jle3DGraph.h"
#include "jleGameEngine.h"
#include "jleResource.h"

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cMesh)


cMesh::cMesh(jleObject *owner, jleScene *scene) : jleComponent(owner, scene) {}

void
cMesh::start()
{}

void
cMesh::update(float dt)
{
    if (_meshRef) {
        std::shared_ptr<jleMesh> mesh = _meshRef.get();
        std::shared_ptr<jleMaterial> material = _materialRef.get();
        gEngine->renderGraph().sendMesh(
            mesh, material, getTransform().getWorldMatrix(), _attachedToObject->instanceID(), true);
    }
}

void
cMesh::editorUpdate(float dt)
{
    update(dt);
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

