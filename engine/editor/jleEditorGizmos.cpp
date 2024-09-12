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

#include "jleEditorGizmos.h"
#include "jleMaterial.h"
#include "jleMesh.h"
#include "jleResourceRef.h"

struct jleEditorGizmos::jleEditorGizmosMeshMaterialHolder {

    jleEditorGizmosMeshMaterialHolder(jleSerializationContext& ctx)
    {
        lightLampMeshRef = jleResourceRef<jleMesh>{jlePath{"ED:gizmos/models/gizmo_lamp.fbx"}, ctx};
        sunMeshRef = jleResourceRef<jleMesh>{jlePath{"ED:gizmos/models/gizmo_sun.fbx"}, ctx};
        cameraMeshRef = jleResourceRef<jleMesh>{jlePath{"ED:gizmos/models/camera/camera.fbx"}, ctx};
        lampMaterialRef = jleResourceRef<jleMaterial>{jlePath{"ED:gizmos/models/lamp.mat"}, ctx};
        sunMaterialRef = jleResourceRef<jleMaterial>{jlePath{"ED:gizmos/models/sun.mat"}, ctx};
        cameraMaterialRef = jleResourceRef<jleMaterial>{jlePath{"ED:gizmos/models/camera/camera.mat"}, ctx};
        selectedObjectMaterialRef = jleResourceRef<jleMaterial>{jlePath{"ED:gizmos/selectedObject.mat"}, ctx};
    }

    jleResourceRef<jleMesh> lightLampMeshRef;
    jleResourceRef<jleMesh> sunMeshRef;
    jleResourceRef<jleMesh> cameraMeshRef;

    jleResourceRef<jleMaterial> lampMaterialRef;
    jleResourceRef<jleMaterial> sunMaterialRef;
    jleResourceRef<jleMaterial> cameraMaterialRef;

    jleResourceRef<jleMaterial> selectedObjectMaterialRef;
};

jleEditorGizmos::jleEditorGizmos(jleSerializationContext& ctx) { _meshMaterialHolder = std::make_unique<jleEditorGizmosMeshMaterialHolder>(ctx); }

std::shared_ptr<jleMesh>
jleEditorGizmos::lightLampMesh()
{
    return _meshMaterialHolder->lightLampMeshRef.get();
}

std::shared_ptr<jleMesh>
jleEditorGizmos::sunMesh()
{
    return _meshMaterialHolder->sunMeshRef.get();
}

std::shared_ptr<jleMesh>
jleEditorGizmos::cameraMesh()
{
    return _meshMaterialHolder->cameraMeshRef.get();
}

std::shared_ptr<jleMaterial>
jleEditorGizmos::lampMaterial()
{
    return _meshMaterialHolder->lampMaterialRef.get();
}

std::shared_ptr<jleMaterial>
jleEditorGizmos::sunMaterial()
{
    return _meshMaterialHolder->sunMaterialRef.get();
}

std::shared_ptr<jleMaterial>
jleEditorGizmos::cameraMaterial()
{
    return _meshMaterialHolder->cameraMaterialRef.get();
}

std::shared_ptr<jleMaterial>
jleEditorGizmos::selectedObjectMaterial()
{
    return _meshMaterialHolder->selectedObjectMaterialRef.get();
}

jleEditorGizmos::~jleEditorGizmos() = default;
