// Copyright (c) 2023. Johan Lind

#ifndef JLE_EDITORGIZMOS_H
#define JLE_EDITORGIZMOS_H

#ifdef BUILD_EDITOR

#include <memory>

class jleMesh;
class jleMaterial;

class jleEditorGizmos
{
public:
    jleEditorGizmos();
    ~jleEditorGizmos();

    std::shared_ptr<jleMesh> lightLampMesh();
    std::shared_ptr<jleMaterial> lampMaterial();

    std::shared_ptr<jleMesh> sunMesh();
    std::shared_ptr<jleMaterial> sunMaterial();

    std::shared_ptr<jleMesh> cameraMesh();
    std::shared_ptr<jleMaterial> cameraMaterial();

private:
    struct jleEditorGizmosMeshMaterialHolder;
    std::unique_ptr<jleEditorGizmosMeshMaterialHolder> _meshMaterialHolder;
};

#endif // BUILD_EDITOR

#endif // JLE_EDITORGIZMOS_H
