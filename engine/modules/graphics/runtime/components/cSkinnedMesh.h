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

#include <core/jleCommon.h>

#include "modules/graphics/jleMaterial.h"
#include "modules/graphics/jleSkinnedMesh.h"

class cAnimator;
class cTransform;

class cSkinnedMesh
{
public:
    template <class Archive>
    void serialize(Archive &ar){
        ar(CEREAL_NVP(_skinnedMeshRef), CEREAL_NVP(_materialRef));
    }

    void ecsUpdate(jleFramePacket &packet, const glm::mat4& worldMatrix, const cAnimator* optionalAnimator, int objectIndex);

    std::shared_ptr<jleSkinnedMesh> getMesh();
    std::shared_ptr<jleMaterial> getMaterial();

    jleResourceRef<jleSkinnedMesh>& getMeshRef();
    jleResourceRef<jleMaterial>& getMaterialRef();

   // void editorInspectorImGuiRender(jleEditorUpdateContext &ctx);

protected:
    jleResourceRef<jleSkinnedMesh> _skinnedMeshRef;
    jleResourceRef<jleMaterial> _materialRef;
};
