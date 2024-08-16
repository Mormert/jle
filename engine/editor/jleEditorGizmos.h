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

#ifndef JLE_EDITORGIZMOS_H
#define JLE_EDITORGIZMOS_H

#include "core/jleCommon.h"

#if JLE_BUILD_EDITOR

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

    std::shared_ptr<jleMaterial> selectedObjectMaterial();

private:
    struct jleEditorGizmosMeshMaterialHolder;
    std::unique_ptr<jleEditorGizmosMeshMaterialHolder> _meshMaterialHolder;
};

#endif // JLE_BUILD_EDITOR

#endif // JLE_EDITORGIZMOS_H
