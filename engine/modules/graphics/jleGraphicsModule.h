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

#include "jleGraphicsModuleTypes.h"

#include "core/jleFramebufferMultisample.h"
#include "core/jleFullscreenRendering.h"
#include "core/jlePath.h"
#include "jleFramePacket.h"
#include "jleGraphics.h"
#include "jleRenderThread.h"

#include "modules/jleGameModules.h"

#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

struct jleCamera;
class jleMesh;
class jleSkinnedMesh;
class jleMeshModule;
class jleImageModule;

namespace jlECS
{
class ECS;
}

class jleGraphicsModule : public jleGameBaseModule
{
public:
    virtual void initializeECS(jlECS::ECS &ecs);
    void initializeModule(jleSerializationContext& ctx);

    virtual void preRender();
    virtual void render(int windowX, int windowY);
    virtual void postRender();

    ~jleGraphicsModule() override;

    void populateSerializeableInterface(std::vector<jleSerializableInterface *> & interfaces);

    struct UpdateContext {
        struct In {
            uint32_t screenX;
            uint32_t screenY;
            const std::vector<glm::mat4>& worldMatrices;
            const jleMeshModule* meshModule;
            const jleImageModule* imageModule;
        } in;

        struct InOut {
            jlECS::ECS &ecs;
        } inOut;
    };

    void update(const UpdateContext &ctx);

    jleGraphics& getGraphics() const { return *_graphics; }

protected:
    virtual void display();

    std::unique_ptr<jleGraphics> _graphics;
    std::unique_ptr<jleFullscreenRendering> _fullscreen_renderer;
    std::unique_ptr<jleFramebufferInterface> _screenFramebuffer;
    std::unique_ptr<jleFramebufferMultisample> _msaaFramebuffer;

    jleFramePacket _framePackets[2]; // Current and previous frame's packet
    int _currentFramePacketIndex = 0;

    jleFramePacket& getCurrentFramePacket() { return _currentFramePacketIndex == 0 ? _framePackets[0] : _framePackets[1]; }
    const jleFramePacket& getPreviousFramePacket() { return _currentFramePacketIndex == 0 ? _framePackets[1] : _framePackets[0]; }

    std::unique_ptr<jleRenderThread> _renderThread;

    std::unordered_map<jlePath, jleMeshGPUDataHandle> _meshGPULookup;
    std::unordered_set<std::shared_ptr<jleMesh>> _meshesToLoadIntoGPU;
    std::vector<jleMeshGPUData> _meshGpuBuffers;

    std::unordered_map<jlePath, jleSkinnedMeshGPUDataHandle> _skinnedMeshGPULookup;
    std::vector<jleSkinnedMeshGPUData> _skinnedMeshGpuBuffers;

    std::unordered_map<jlePath, jleMaterialGPUDataHandle> _materialGPULookup;
    std::unordered_set<jlePath> _materialsToLoadIntoGPU;
    std::vector<jleMaterialGPUData> _materialGpuBuffers;

    std::unordered_map<jlePath, jleTextureGPUDataHandle> _textureGPULookup;
    std::unordered_set<jlePath> _texturesToLoadIntoGPU;

    static void createMeshGPUBuffers(jleMeshGPUData* gpuData, const jleMesh* mesh);
    static void destroyMeshGPUBuffers(jleMeshGPUData* gpuData);

    static void createSkinnedMeshGPUBuffers(jleSkinnedMeshGPUData* gpuData, jleSkinnedMesh* mesh);
    static void destroySkinnedMeshGPUBuffers(jleSkinnedMeshGPUData* gpuData);

    static void createMaterialGPUData(jleMaterialGPUData* gpuData, std::shared_ptr<jleMaterial> material, const jlePath& path);
    static void destroyMaterialGPUData(jleMaterialGPUData* gpuData);

    friend class jleEditor;

    jleResourceHolder _resourceHolder;

    // TODO: Remove when we merge with jleGraphics.
    friend class jleGraphics;
};
