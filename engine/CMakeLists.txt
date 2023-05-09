# Copyright (c) 2022. Johan Lind

add_subdirectory(3rdparty/zlib)

set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
set(ZLIB_LIBRARY zlib)
set(ZLIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/zlib)
set(SKIP_INSTALL_ALL ON)

include_directories(${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
find_package(ZLIB REQUIRED)

add_library(engine STATIC
        "3rdparty/stb_image.cpp"
        "3rdparty/tiny_obj_loader.cpp"
        "3rdparty/Remotery/Remotery.c"
        "jleImage.cpp"
        "jleImageFlipped.cpp"
        "jleComponent.cpp"
        "jleCore.cpp"
        "jleEngineStatus.cpp"
        "jleGame.cpp"
        "jleObject.cpp"
        "jleAseprite.cpp"
        "jleScene.cpp"
        "jleTextRendering.cpp"
        "jleStaticOpenGLState.cpp"
        "jleGameEngine.cpp"
        "jleKeyboardInput.cpp"
        "jleMouseInput.cpp"
        "jleFrameBufferInterface.cpp"
        "jleFramebufferMultisample.cpp"
        "jleFramebufferShadowMap.cpp"
        "jleFramebufferShadowCubeMap.cpp"
        "jleFramebufferPicking.cpp"
        "jleFullscreenRendering.cpp"
        "jleQuadRendering.cpp"
        "jleRendering.cpp"
        "jleShader.cpp"
        "jleTexture.cpp"
        "jleWindow.cpp"
        "jleTransform.cpp"
        "cSprite.cpp"
        "cSpriteDepth.cpp"
        "cAseprite.cpp"
        "cCamera.cpp"
        "jleFont.cpp"
        "cText.cpp"
        "jleTimerManager.cpp"
        "cUITransformUpdater.cpp"
        "jleNetworking.cpp"
        "jleNetworkingNative.cpp"
        "jleNetworkingJS.cpp"
        "jleNetScene.cpp"
        "jleSpritesheet.cpp"
        "cSpritesheetDepth.cpp"
        "cSpritesheet.cpp"
        "jlePath.cpp"
        "jleProfiler.cpp"
        "jleCamera.cpp"
        "jle3DRenderer.cpp"
        "cCameraFPV.cpp"
        "jleMesh.cpp"
        "cMesh.cpp"
        "jleSkybox.cpp"
        "cSkybox.cpp"
        "cLight.cpp"
        "cLightDirectional.cpp"
        "jleFramebufferScreen.cpp"
        "jleMaterial.cpp"
        "jleExplicitInclude.cpp")

if (BUILD_EDITOR)
    target_sources(engine PRIVATE
            "editor/jleEditor.cpp"
            "editor/jleEditorResourceViewer.cpp"
            "editor/jleEditorResourceViewer.cpp"
            "editor/jleEditorSettingsWindow.cpp"
            "editor/jleConsoleEditorWindow.cpp"
            "editor/jleEditorGameControllerWidget.cpp"
            "editor/jleEditorSceneObjectsWindow.cpp"
            "editor/jleEditorProfilerWindow.cpp"
            "editor/jleEditorResourceViewer.cpp"
            "editor/jleSceneEditorWindow.cpp"
            "editor/jleGameEditorWindow.cpp"
            "editor/jleEditorContentBrowser.cpp"
            "editor/jleProfilerWindow.cpp"
            "editor/jleEditorWindowsPanel.cpp"
            "editor/jleEditorTextEdit.cpp"
            "editor/jleEditorResourceEdit.cpp"
            "3rdparty/ImGuiColorTextEdit/TextEditor.cpp"
            "3rdparty/ImGui/imgui.cpp"
            "3rdparty/ImGui/imgui_spectrum.cpp"
            "3rdparty/ImGui/imgui_demo.cpp"
            "3rdparty/ImGui/imgui_draw.cpp"
            "3rdparty/ImGui/imgui_impl_glfw.cpp"
            "3rdparty/ImGui/imgui_impl_opengl3.cpp"
            "3rdparty/ImGui/imgui_tables.cpp"
            "3rdparty/ImGui/imgui_widgets.cpp"
            "3rdparty/ImGui/imgui_stdlib.cpp"
            "3rdparty/ImGui/ImGuizmo.cpp")

            set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
            set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
            set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
            set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
            set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
            add_subdirectory(3rdparty/git_submodules/assimp)
            target_link_libraries(engine PUBLIC assimp)
            target_include_directories(engine SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules/assimp/include")
endif ()

target_link_libraries(engine PUBLIC zlib)


if (NOT BUILD_EMSCRIPTEN) # Native build, excluding some Emscripten-embedded libraries else()

    target_link_libraries(engine PUBLIC ${CMAKE_DL_LIBS})

    # Compile glad.c only when building native
    target_sources(engine PRIVATE "3rdparty/glad/glad.c")

    # Remove GLFW tests, examples and docs from build
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory(3rdparty/git_submodules/glfw)
    target_link_libraries(engine PUBLIC glfw)

    add_subdirectory(3rdparty/git_submodules/socket.io-client-cpp)
    target_link_libraries(engine PUBLIC sioclient)
    target_include_directories(engine SYSTEM PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/socket.io-cpp/src")

endif ()

add_subdirectory(3rdparty/SoLoud)
target_link_libraries(engine PUBLIC SoLoud)

# Disable HarfBuzz Dependency
set(FT_DISABLE_HARFBUZZ ON)
set(FT_REQUIRE_ZLIB ON)
add_subdirectory(3rdparty/freetype)
target_link_libraries(engine PUBLIC freetype)

target_include_directories(engine SYSTEM PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty"
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules/plog/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules/glm")

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (BUILD_EMSCRIPTEN)
    # When building with Emscripten, it needs to find the pre-loaded assets
    # at the same location as the 'binary'
    FILE(COPY EditorResources DESTINATION ${PROJECT_BINARY_DIR})
    FILE(COPY EngineResources DESTINATION ${PROJECT_BINARY_DIR})
else ()
    FILE(COPY EditorResources DESTINATION ${PROJECT_BINARY_DIR}/${JLE_GAME_BUILD})
    FILE(COPY EngineResources DESTINATION ${PROJECT_BINARY_DIR}/${JLE_GAME_BUILD})
endif ()
