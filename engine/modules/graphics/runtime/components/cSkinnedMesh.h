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

#include "jleBuildConfig.h"

#include "jleComponent.h"

#include "jleSkinnedMesh.h"
#include "jleMaterial.h"

class cAnimator;

class cSkinnedMesh : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cSkinnedMesh)
public:

    template <class Archive>
    void serialize(Archive &ar);

    void editorUpdate(float dt) override;

    void start() override;

    void update(float dt) override;

    std::shared_ptr<jleSkinnedMesh> getMesh();
    std::shared_ptr<jleMaterial> getMaterial();

    jleResourceRef<jleSkinnedMesh>& getMeshRef();
    jleResourceRef<jleMaterial>& getMaterialRef();

    void editorInspectorImGuiRender() override;

protected:

    jleResourceRef<jleSkinnedMesh> _skinnedMeshRef;
    jleResourceRef<jleMaterial> _materialRef;

    std::shared_ptr<cAnimator> _animator;

    void findAnimator(jleObject* object);

};

JLE_EXTERN_TEMPLATE_CEREAL_H(cSkinnedMesh)

CEREAL_REGISTER_TYPE(cSkinnedMesh)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cSkinnedMesh)