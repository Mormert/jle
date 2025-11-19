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

#include "jleGraphicsModuleEditorUI.h"
#include "jleGraphicsModuleEditor.h"
#include "modules/graphics/core/jleIncludeGL.h"
#include "modules/image/editor/jleImageModuleEditor.h"
#include "modules/mesh/editor/jleMeshModuleEditor.h"
#include "modules/graphics/core/jleFramebufferScreen.h"
#include "core/jleCamera.h"
#include "modules/graphics/jleGraphics.h"

#include <ImGui/imgui.h>
#include <algorithm>
#include <cmath>

void
jleGraphicsModuleEditorUI::openImageEditorResourcePrompt(const jlePath &path)
{
    const float globalImguiScale = ImGui::GetIO().FontGlobalScale;
    const ImVec2 size{100 * globalImguiScale, 25 * globalImguiScale};

    if (ImGui::Button("View Image", size)) {
        _openedImagesInEditor.insert(path);
    }
}

void
jleGraphicsModuleEditorUI::openMeshEditorResourcePrompt(const jlePath &path)
{
    const float globalImguiScale = ImGui::GetIO().FontGlobalScale;
    const ImVec2 size{100 * globalImguiScale, 25 * globalImguiScale};

    if (ImGui::Button("View Mesh", size)) {
        _openedMeshesInEditor.insert(path);
    }
}

void jleGraphicsModuleEditorUI::renderUI(jleImageModuleEditor& imageModule, jleGraphicsModuleEditor& graphicsModule, jleMeshModuleEditor& meshModule)
{
    std::vector<jlePath> imagesToClose;
    std::vector<jlePath> meshesToClose;
    
    // Render image viewers
    for (const auto& path : _openedImagesInEditor)
    {
        auto it = graphicsModule._textureGPULookup.find(path);
        auto image = imageModule.loadImageSync(path);

        if (it == graphicsModule._textureGPULookup.end())
        {
            graphicsModule._texturesToLoadIntoGPU.insert(path);
            continue;
        }

        uint32_t texId = it->second;

        std::string windowName = "Image: " + path.getVirtualPath().str();
        
        auto& state = _imageViewStates[path];
        const int w = image->width();
        const int h = image->height();

        if (state.needsAutoFit)
        {
            constexpr float maxImageWidth = 800.0f;
            constexpr float maxImageHeight = 600.0f;
            constexpr float uiPadding = 150.0f;
            constexpr float windowPadding = 50.0f;

            float scaleX = maxImageWidth / w;
            float scaleY = maxImageHeight / h;
            float fitScale = std::min(scaleX, scaleY) * 0.9f;

            float fittedWidth = w * fitScale;
            float fittedHeight = h * fitScale;

            float windowWidth = fittedWidth + windowPadding;
            float windowHeight = fittedHeight + uiPadding;

            windowWidth = std::max(windowWidth, 400.0f);
            windowHeight = std::max(windowHeight, 300.0f);

            ImVec2 displaySize = ImGui::GetIO().DisplaySize;
            ImVec2 windowPos = ImVec2((displaySize.x - windowWidth) * 0.5f, (displaySize.y - windowHeight) * 0.5f);
            
            ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(windowPos, ImGuiCond_FirstUseEver);
        }
        
        bool windowOpen = true;
        if (!ImGui::Begin(windowName.c_str(), &windowOpen))
        {
            ImGui::End();
            if (!windowOpen)
            {
                imagesToClose.push_back(path);
            }
            continue;
        }
        
        if (!windowOpen)
        {
            imagesToClose.push_back(path);
        }

        if (ImGui::Button("Zoom +")) state.zoom *= 1.1f;
        ImGui::SameLine();
        if (ImGui::Button("Zoom -")) state.zoom /= 1.1f;
        ImGui::SameLine();
        if (ImGui::Button("Reset")) { state.zoom = 1.0f; state.pan = glm::vec2(0, 0);
        }
        ImGui::SameLine();
        if (ImGui::Button("Fit")) {
            ImVec2 avail = ImGui::GetContentRegionAvail();
            float scale = std::min(avail.x / w, avail.y / h);
            state.zoom = scale;
            state.pan = glm::vec2(0,0);
        }

        ImGui::SliderFloat("Zoom", &state.zoom, 0.05f, 10.0f, "%.2f");
        ImGui::Checkbox("Flip X", &state.flipX);
        ImGui::SameLine();
        ImGui::Checkbox("Flip Y", &state.flipY);
        ImGui::Checkbox("Checkerboard", &state.showChecker);
        ImGui::Checkbox("Lock Aspect", &state.lockAspect);

        ImGui::Separator();
        ImGui::Text("Texture Filtering:");
        
        const char* filterNames[] = {"Nearest", "Linear", "Nearest Mipmap", "Linear Mipmap"};
        int minFilterIndex = static_cast<int>(state.minFilter);
        int magFilterIndex = static_cast<int>(state.magFilter);
        
        if (ImGui::Combo("Min Filter", &minFilterIndex, filterNames, 4)) {
            state.minFilter = static_cast<jleGraphicsModuleEditorUI::TextureFilter>(minFilterIndex);
        }
        if (ImGui::Combo("Mag Filter", &magFilterIndex, filterNames, 2)) { // Only Nearest and Linear for mag filter
            state.magFilter = static_cast<jleGraphicsModuleEditorUI::TextureFilter>(magFilterIndex);
        }
        
        ImGui::Separator();
        ImGui::Text("Dimensions: %d x %d", w, h);

        ImVec2 availableSize = ImGui::GetContentRegionAvail();
        if (ImGui::BeginChild("ImageViewport", availableSize, true, ImGuiWindowFlags_NoScrollbar))
        {
            ImVec2 childSize = ImGui::GetContentRegionAvail();
            ImVec2 cursor = ImGui::GetCursorScreenPos();
            
            // Auto-fit image on first display
            if (state.needsAutoFit && childSize.x > 0 && childSize.y > 0) {
                float scaleX = childSize.x / w;
                float scaleY = childSize.y / h;
                state.zoom = std::min(scaleX, scaleY) * 0.9f;
                state.pan = glm::vec2(0, 0);
                state.needsAutoFit = false;
            }
            
            ImVec2 size = ImVec2(w * state.zoom, h * state.zoom);

            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0 && ImGui::IsWindowHovered())
            {
                float prevZoom = state.zoom;
                state.zoom *= (1.0f + wheel * 0.1f);

                // Zoom around mouse position
                ImVec2 mouse = ImGui::GetMousePos();
                ImVec2 local = ImVec2(mouse.x - cursor.x, mouse.y - cursor.y);

                state.pan.x -= local.x * (state.zoom - prevZoom);
                state.pan.y -= local.y * (state.zoom - prevZoom);
            }

            // Panning
            if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right))
            {
                ImVec2 delta = ImGui::GetIO().MouseDelta;
                state.pan.x += delta.x;
                state.pan.y += delta.y;
            }

            float maxPan = 500.0f;
            state.pan.x = glm::clamp(state.pan.x, -maxPan, maxPan);
            state.pan.y = glm::clamp(state.pan.y, -maxPan, maxPan);

            // Draw checker background
            if (state.showChecker)
            {
                ImDrawList* dl = ImGui::GetWindowDrawList();
                ImVec2 imagePos = ImVec2(cursor.x + state.pan.x, cursor.y + state.pan.y);
                dl->AddRectFilled(imagePos, ImVec2(imagePos.x + size.x, imagePos.y + size.y),
                                  IM_COL32(50, 50, 50, 255));
                const float checker = 16.0f * state.zoom;
                for (float y = 0; y < size.y; y += checker)
                    for (float x = 0; x < size.x; x += checker)
                        if (((int)(x/checker) + (int)(y/checker)) % 2 == 0)
                            dl->AddRectFilled(ImVec2(imagePos.x + x, imagePos.y + y),
                                              ImVec2(imagePos.x + x + checker, imagePos.y + y + checker),
                                              IM_COL32(80, 80, 80, 255));
            }

            glBindTexture(GL_TEXTURE_2D, texId);
            
            // Convert filter enum to OpenGL constants
            auto getGLFilter = [](jleGraphicsModuleEditorUI::TextureFilter filter, bool isMag) {
                switch (filter) {
                    case jleGraphicsModuleEditorUI::TextureFilter::Nearest:
                        return GL_NEAREST;
                    case jleGraphicsModuleEditorUI::TextureFilter::Linear:
                        return GL_LINEAR;
                    case jleGraphicsModuleEditorUI::TextureFilter::NearestMipmap:
                        return isMag ? GL_NEAREST : GL_NEAREST_MIPMAP_NEAREST;
                    case jleGraphicsModuleEditorUI::TextureFilter::LinearMipmap:
                        return isMag ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR;
                    default:
                        return GL_LINEAR;
                }
            };
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getGLFilter(state.minFilter, false));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getGLFilter(state.magFilter, true));

            ImVec2 uv0 = ImVec2(state.flipX ? 1 : 0, state.flipY ? 1 : 0);
            ImVec2 uv1 = ImVec2(state.flipX ? 0 : 1, state.flipY ? 0 : 1);

            ImGui::SetCursorScreenPos(ImVec2(cursor.x + state.pan.x, cursor.y + state.pan.y));
            ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texId)), size, uv0, uv1);

            glBindTexture(GL_TEXTURE_2D, 0);

            // Pixel inspection
            ImVec2 mouse = ImGui::GetMousePos();
            ImVec2 imgPos = ImVec2(cursor.x + state.pan.x, cursor.y + state.pan.y);

            bool inside =
                mouse.x >= imgPos.x && mouse.x < imgPos.x + size.x &&
                mouse.y >= imgPos.y && mouse.y < imgPos.y + size.y &&
                ImGui::IsWindowHovered();

            if (inside) {
                float u = (mouse.x - imgPos.x) / size.x;
                float v = (mouse.y - imgPos.y) / size.y;
                int px = static_cast<int>(u * w);
                int py = static_cast<int>(v * h);

                ImGui::SetCursorScreenPos(ImVec2(cursor.x + 10, cursor.y + 10));
                ImGui::Text("Pixel: (%d, %d)  UV:(%.3f, %.3f)", px, py, u, v);
            }
        }
        ImGui::EndChild();

        ImGui::End();
    }
    
    // Render mesh viewers
    for (const auto& path : _openedMeshesInEditor)
    {
        auto mesh = meshModule.loadMeshSync(path);
        if (!mesh) {
            continue;
        }

        std::string windowName = "Mesh: " + path.getVirtualPath().str();
        
        auto& state = _meshViewStates[path];
        
        // Set initial window size for mesh viewer
        if (state.isFirstFrame)
        {
            constexpr float windowWidth = 800.0f;
            constexpr float windowHeight = 600.0f;
            
            ImVec2 displaySize = ImGui::GetIO().DisplaySize;
            ImVec2 windowPos = ImVec2((displaySize.x - windowWidth) * 0.5f, (displaySize.y - windowHeight) * 0.5f);
            
            ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(windowPos, ImGuiCond_FirstUseEver);
            state.isFirstFrame = false;
        }
        
        bool windowOpen = true;
        if (!ImGui::Begin(windowName.c_str(), &windowOpen))
        {
            ImGui::End();
            if (!windowOpen)
            {
                meshesToClose.push_back(path);
            }
            continue;
        }
        
        if (!windowOpen)
        {
            meshesToClose.push_back(path);
        }

        if (ImGui::Button("Reset Camera")) {
            state.cameraDistance = 3.0f;
            state.cameraYaw = 0.0f;
            state.cameraPitch = 0.0f;
            state.cameraPosition = {0.0f, 0.0f, 3.0f};
            state.cameraTarget = {0.0f, 0.0f, 0.0f};
        }
        ImGui::SameLine();
        ImGui::Checkbox("Wireframe", &state.showWireframe);
        ImGui::SameLine();
        ImGui::Checkbox("Auto Rotate", &state.autoRotate);
        
        ImGui::Separator();

        ImGui::Text("Triangles: %d", mesh->getTrianglesCount());

        // 3D viewport
        ImVec2 availableSize = ImGui::GetContentRegionAvail();
        if (ImGui::BeginChild("MeshViewport", availableSize, true, ImGuiWindowFlags_NoScrollbar))
        {
            // Update camera position based on spherical coordinates
            float yawRad = glm::radians(state.cameraYaw);
            float pitchRad = glm::radians(state.cameraPitch);
            
            state.cameraPosition.x = state.cameraDistance * cosf(pitchRad) * cosf(yawRad);
            state.cameraPosition.y = state.cameraDistance * sinf(pitchRad);
            state.cameraPosition.z = state.cameraDistance * cosf(pitchRad) * sinf(yawRad);
            
            // Auto rotation
            if (state.autoRotate) {
                state.meshRotation.y += state.autoRotateSpeed * ImGui::GetIO().DeltaTime * 57.2958f; // Convert to degrees
                if (state.meshRotation.y > 360.0f) state.meshRotation.y -= 360.0f;
            }
            
            // Mouse camera controls
            bool isLeftDragging = ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left);
            bool isRightDragging = ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right);
            
            if (isLeftDragging || isRightDragging)
            {
                if (!state.isDragging) {
                    state.isDragging = true;
                    state.lastMousePos = ImGui::GetMousePos();
                } else {
                    ImVec2 currentMousePos = ImGui::GetMousePos();
                    ImVec2 mouseDelta = ImVec2(currentMousePos.x - state.lastMousePos.x, 
                                              currentMousePos.y - state.lastMousePos.y);
                    
                    if (isLeftDragging) {
                        // Left mouse: Pan camera target
                        float sensitivity = 0.002f * state.cameraDistance; // Slower, zoom-dependent panning
                        
                        // Calculate camera right and up vectors
                        glm::vec3 forward = glm::normalize(state.cameraTarget - state.cameraPosition);
                        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
                        glm::vec3 up = glm::normalize(glm::cross(right, forward));
                        
                        // Pan the target
                        state.cameraTarget -= right * (mouseDelta.x * sensitivity);
                        state.cameraTarget += up * (mouseDelta.y * sensitivity);
                    } else if (isRightDragging) {
                        // Right mouse: Orbit camera
                        state.cameraYaw += mouseDelta.x * 0.5f;
                        state.cameraPitch -= mouseDelta.y * 0.5f;
                        state.cameraPitch = glm::clamp(state.cameraPitch, -89.0f, 89.0f);
                    }
                    
                    state.lastMousePos = currentMousePos;
                }
            } else {
                state.isDragging = false;
            }
            
            // Mouse wheel zoom
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0 && ImGui::IsWindowHovered()) {
                state.cameraDistance -= wheel * 0.5f;
                state.cameraDistance = glm::clamp(state.cameraDistance, 0.5f, 400.0f); // 20x more zoom out
            }
            
            // Set up 3D rendering
            ImVec2 canvasSize = ImGui::GetContentRegionAvail();
            const uint32_t viewportWidth = static_cast<uint32_t>(canvasSize.x);
            const uint32_t viewportHeight = static_cast<uint32_t>(canvasSize.y);
            
            // Create or update framebuffer for 3D rendering
            if (!state.framebuffer || 
                state.framebuffer->width() != viewportWidth || 
                state.framebuffer->height() != viewportHeight) {
                state.framebuffer = std::make_unique<jleFramebufferScreen>(viewportWidth, viewportHeight);
            }
            
            // Set up camera for 3D rendering
            jleCamera camera3D;
            glm::mat4 viewMatrix = glm::lookAt(state.cameraPosition, state.cameraTarget, state.cameraUp);
            camera3D.setViewMatrix(viewMatrix);
            camera3D.setPerspectiveProjection(45.0f, viewportWidth, viewportHeight, 0.1f, 100.0f);
            
            // Create a minimal frame packet for rendering the mesh
            jleFramePacket framePacket;

            glm::mat4 meshTransform = glm::mat4(1.0f);
            meshTransform = glm::translate(meshTransform, state.meshPosition);
            meshTransform = glm::rotate(meshTransform, glm::radians(state.meshRotation.x), glm::vec3(1, 0, 0));
            meshTransform = glm::rotate(meshTransform, glm::radians(state.meshRotation.y), glm::vec3(0, 1, 0));
            meshTransform = glm::rotate(meshTransform, glm::radians(state.meshRotation.z), glm::vec3(0, 0, 1));
            meshTransform = glm::scale(meshTransform, state.meshScale);

            auto meshIt = graphicsModule._meshGPULookup.find(path);
            if (meshIt != graphicsModule._meshGPULookup.end()) {
                // TODO: For now, we use an invalid material handle as we just want to see the mesh
                const jleMaterialGPUDataHandle invalidTempHandle = jleMaterialGPUDataHandle();
                framePacket.sendMesh(meshIt->second, invalidTempHandle, meshTransform, 0, state.showWireframe);
            } else {
                graphicsModule._meshesToLoadIntoGPU.insert(mesh);
            }

            graphicsModule.getGraphics().render(*state.framebuffer, framePacket, &camera3D);

            uint32_t textureId = state.framebuffer->texture();
            ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(textureId)), 
                        canvasSize, ImVec2(0, 1), ImVec2(1, 0));

            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 canvasPos = ImGui::GetCursorScreenPos();
            drawList->AddText(ImVec2(canvasPos.x + 10, canvasPos.y + 10), IM_COL32(255, 255, 255, 255),
                             ("Camera: Yaw=" + std::to_string(static_cast<int>(state.cameraYaw)) + 
                              " Pitch=" + std::to_string(static_cast<int>(state.cameraPitch)) +
                              " Dist=" + std::to_string(state.cameraDistance)).c_str());
        }
        ImGui::EndChild();

        ImGui::End();
    }

    for (const auto& pathToClose : imagesToClose)
    {
        _openedImagesInEditor.erase(pathToClose);
        _imageViewStates.erase(pathToClose);
    }

    for (const auto& pathToClose : meshesToClose)
    {
        _openedMeshesInEditor.erase(pathToClose);
        _meshViewStates.erase(pathToClose);
    }
}