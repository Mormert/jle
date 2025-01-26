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
    void postRender() override;

    void initializeECS(jlECS::ECS &ecs) override;

    void updateEditor(jleEditorUpdateContext& ctx, const std::vector<glm::mat4>& worldMatrices);

    std::unique_ptr<jleFramebufferScreen> cameraPreviewFramebuffer;

    void setGameWindowSize(uint32_t width, uint32_t height);

    uint32_t getGameWindowTextureId() const { return _screenFramebuffer->texture(); };

    jleFramePacket& getCurrentFramePacketEditor() { return _currentFramePacketIndex == 0 ? _framePacketsEditor[0] : _framePacketsEditor[1];  }
    const jleFramePacket& getPreviousFramePacketEditor() { return _currentFramePacketIndex == 0 ? _framePacketsEditor[1] : _framePacketsEditor[0]; }
    const jleFramePacket& getPreviousFramePacketGame() { return getPreviousFramePacket(); }

protected:
    void display() override;

private:
    jleFramePacket _framePacketsEditor[2]; // Current and previous frame's editor packet
};