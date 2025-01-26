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

#include "cCamera.h"

#include "core/jleCamera.h"
#include "modules/hierarchy/components/cTransform.h"

void
cCamera::update(UpdateContext& ctx) const
{
    const uint32_t width = ctx.in.width;
    const uint32_t height = ctx.in.height;

    if (perspective && width > 0 && height > 0) {
        ctx.out.camera.setPerspectiveProjection(perspectiveFov, width, height, farPlane, nearPlane);
    } else {
        ctx.out.camera.setOrthographicProjection(width, height, farPlane, nearPlane);
    }

    const auto &transformation = ctx.in.transform.getLocalMatrix();
    ctx.out.camera.setViewMatrix(glm::inverse(transformation));
}


