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

#include "modules/graphics/jleMaterial.h"
#include "modules/graphics/jleMesh.h"

class cTransform;

class cMesh
{
public:
    template <class Archive>
    void
    serialize(Archive &ar){
        ar(CEREAL_NVP(_meshRef), CEREAL_NVP(_materialRef));
    }

    void ecsUpdate(jleFramePacket &packet, const cTransform& transform, int instanceId);

    std::shared_ptr<jleMesh> getMesh();
    std::shared_ptr<jleMaterial> getMaterial();

    jleResourceRef<jleMesh> &getMeshRef();
    jleResourceRef<jleMaterial> &getMaterialRef();

protected:
    jleResourceRef<jleMesh> _meshRef;
    jleResourceRef<jleMaterial> _materialRef;
};

