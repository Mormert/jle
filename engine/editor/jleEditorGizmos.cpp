// Copyright (c) 2023. Johan Lind

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
        cameraMaterialRef = jleResourceRef<jleMaterial>{jlePath{"ED:gizmos/models/camera/camera.mat"}};
    }

    jleResourceRef<jleMesh> lightLampMeshRef;
    jleResourceRef<jleMesh> sunMeshRef;
    jleResourceRef<jleMesh> cameraMeshRef;
    jleResourceRef<jleMaterial> cameraMaterialRef;
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
jleEditorGizmos::cameraMaterial()
{
    return _meshMaterialHolder->cameraMaterialRef.get();
}

jleEditorGizmos::~jleEditorGizmos() = default;
