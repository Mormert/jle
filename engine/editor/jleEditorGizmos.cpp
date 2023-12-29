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

    jleEditorGizmosMeshMaterialHolder()
    {
        lightLampMeshRef = jleResourceRef<jleMesh>{jlePath{"ED:gizmos/models/gizmo_lamp.fbx"}};
        sunMeshRef = jleResourceRef<jleMesh>{jlePath{"ED:gizmos/models/gizmo_sun.fbx"}};
        cameraMeshRef = jleResourceRef<jleMesh>{jlePath{"ED:gizmos/models/camera/camera.fbx"}};
        lampMaterialRef = jleResourceRef<jleMaterial>{jlePath{"ED:gizmos/models/lamp.mat"}};
        sunMaterialRef = jleResourceRef<jleMaterial>{jlePath{"ED:gizmos/models/sun.mat"}};
        cameraMaterialRef = jleResourceRef<jleMaterial>{jlePath{"ED:gizmos/models/camera/camera.mat"}};
        selectedObjectMaterialRef = jleResourceRef<jleMaterial>{jlePath{"ED:gizmos/selectedObject.mat"}};
    }

    jleResourceRef<jleMesh> lightLampMeshRef;
    jleResourceRef<jleMesh> sunMeshRef;
    jleResourceRef<jleMesh> cameraMeshRef;

    jleResourceRef<jleMaterial> lampMaterialRef;
    jleResourceRef<jleMaterial> sunMaterialRef;
    jleResourceRef<jleMaterial> cameraMaterialRef;

    jleResourceRef<jleMaterial> selectedObjectMaterialRef;
};

jleEditorGizmos::jleEditorGizmos() { _meshMaterialHolder = std::make_unique<jleEditorGizmosMeshMaterialHolder>(); }

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
