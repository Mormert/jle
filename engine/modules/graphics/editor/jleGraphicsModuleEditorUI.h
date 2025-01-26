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

#include <core/jlePath.h>
#include <unordered_set>
#include <unordered_map>
#include <glm/glm.hpp>
#include <ImGui/imgui.h>

class jleGraphicsModuleEditor;
class jleImageModuleEditor;

class jleGraphicsModuleEditorUI
{
public:
    void openImageEditorResourcePrompt(const jlePath& path);
    void openMeshEditorResourcePrompt(const jlePath& path);

    void renderUI(jleImageModuleEditor& imageModule, jleGraphicsModuleEditor& graphicsModule, class jleMeshModuleEditor& meshModule);

private:
    std::unordered_set<jlePath> _openedImagesInEditor;
    std::unordered_set<jlePath> _openedMeshesInEditor;

    enum class TextureFilter {
        Nearest = 0,
        Linear = 1,
        NearestMipmap = 2,
        LinearMipmap = 3
    };

    struct ImageViewState {
        float zoom = 1.0f;
        glm::vec2 pan = {};
        bool flipX = false;
        bool flipY = false;
        bool showChecker = true;
        bool lockAspect = true;
        TextureFilter minFilter = TextureFilter::Linear;
        TextureFilter magFilter = TextureFilter::Linear;
        bool needsAutoFit = true;
    };

    struct MeshViewState {
        glm::vec3 cameraPosition = {0.0f, 0.0f, 3.0f};
        glm::vec3 cameraTarget = {0.0f, 0.0f, 0.0f};
        glm::vec3 cameraUp = {0.0f, 1.0f, 0.0f};
        float cameraDistance = 3.0f;
        float cameraYaw = 0.0f;
        float cameraPitch = 0.0f;
        bool showWireframe = false;
        bool showGrid = true;
        bool autoRotate = false;
        float autoRotateSpeed = 1.0f;
        glm::vec3 meshRotation = {0.0f, 0.0f, 0.0f};
        glm::vec3 meshScale = {1.0f, 1.0f, 1.0f};
        glm::vec3 meshPosition = {0.0f, 0.0f, 0.0f};
        bool isFirstFrame = true;
        bool isDragging = false;
        ImVec2 lastMousePos = {0.0f, 0.0f};
        std::unique_ptr<class jleFramebufferScreen> framebuffer;
    };

    std::unordered_map<jlePath, ImageViewState> _imageViewStates;
    std::unordered_map<jlePath, MeshViewState> _meshViewStates;
};