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

#include <modules/jleEditorUpdateContext.h>
#include <modules/graphics/jleGraphicsModule.h>
#include <modules/graphics/core/jleFramebufferScreen.h>

class jleGraphicsModuleEditor : public jleGraphicsModule
{
public:
    void initializeECS(jlECS::ECS &ecs) override;

    void updateEditor(jleEditorUpdateContext& ctx, const std::vector<glm::mat4>& worldMatrices);

    std::unique_ptr<jleFramebufferScreen> cameraPreviewFramebuffer;
};