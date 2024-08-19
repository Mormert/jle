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

#include "jleComponent.h"
#include "jleTransform.h"
#include "modules/graphics/core/jleFrameBufferInterface.h"

class cCamera : public jleComponent
{
    JLE_REGISTER_COMPONENT_TYPE(cCamera)
public:

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(perspective),
           CEREAL_NVP(farPlane),
           CEREAL_NVP(nearPlane),
           CEREAL_NVP(perspectiveFov),
           CEREAL_NVP(framebufferSizeX),
           CEREAL_NVP(framebufferSizeY),
           //CEREAL_NVP(_framebufferFixedAxis),
           CEREAL_NVP(framebufferUseFixedAxis),
           CEREAL_NVP(matchFramebufferToWindowSize));
    }

    ~cCamera() override;

    void start(jleEngineModulesContext& ctx) override;

    void update(jleEngineModulesContext& ctx) override;

    void editorInspectorImGuiRender() override;

    void framebufferResizeCallback(unsigned int width, unsigned int height);

    void editorGizmosRender(bool selected, jle3DGraph& renderGraph) override;

    bool perspective{true};
    float perspectiveFov{90.f};
    float farPlane{10000.f};
    float nearPlane{0.1f};
    int framebufferSizeX{1024};
    int framebufferSizeY{1024};
    bool framebufferUseFixedAxis{false};
    bool matchFramebufferToWindowSize{false};
    jleFramebufferInterface::FIXED_AXIS framebufferFixedAxis{jleFramebufferInterface::FIXED_AXIS::width};

protected:
    int _framebufferCallbackId;

    inline static uint32_t sInstanceCounter = 0;
};

JLE_EXTERN_TEMPLATE_CEREAL_H(cCamera)

CEREAL_REGISTER_TYPE(cCamera)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleComponent, cCamera)