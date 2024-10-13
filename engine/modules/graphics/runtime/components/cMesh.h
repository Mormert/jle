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

#include "core/jleComponent.h"
#include "core/jleTransform.h"
#include "modules/graphics/jleMaterial.h"
#include "modules/graphics/jleMesh.h"

#include "core/jleResourceRef.h"

class cMesh : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cMesh)
public:

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_meshRef), CEREAL_NVP(_materialRef));
    }

    void editorUpdate(jleEngineModulesContext& ctx) override;

    void start(jleEngineModulesContext& ctx) override;

    void update(jleEngineModulesContext& ctx) override;

    std::shared_ptr<jleMesh> getMesh();
    std::shared_ptr<jleMaterial> getMaterial();

    jleResourceRef<jleMesh>& getMeshRef();
    jleResourceRef<jleMaterial>& getMaterialRef();

protected:

    jleResourceRef<jleMesh> _meshRef;
    jleResourceRef<jleMaterial> _materialRef;

};

JLE_EXTERN_TEMPLATE_CEREAL_H(cMesh)

CEREAL_REGISTER_TYPE(cMesh)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cMesh)
