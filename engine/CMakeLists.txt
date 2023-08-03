# Copyright (c) 2022. Johan Lind

add_subdirectory(3rdparty/zlib)

set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
set(ZLIB_LIBRARY zlib)
set(ZLIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/zlib)
set(SKIP_INSTALL_ALL ON)

option(ENABLE_CLANG_TIMETRACE "Enable -ftime-trace for Clang" OFF)
if (ENABLE_CLANG_TIMETRACE AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-ftime-trace ${CMAKE_CXX_FLAGS}")
endif ()

include_directories(${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
find_package(ZLIB REQUIRED)

add_library(engine STATIC
        "3rdparty/stb_image.cpp"
        "3rdparty/tiny_obj_loader.cpp"
        "3rdparty/Remotery/Remotery.cpp"
        "RmlUiImplementation/RmlUi_Backend_GLFW_GL3.cpp"
        "RmlUiImplementation/RmlUi_Renderer_GL3.cpp"
        "RmlUiImplementation/RmlUi_Platform_GLFW.cpp"
        "RmlUiImplementation/shell/src/PlatformExtensions.cpp"
        "RmlUiImplementation/shell/src/RendererExtensions.cpp"
        "RmlUiImplementation/shell/src/Shell.cpp"
        "RmlUiImplementation/shell/src/ShellFileInterface.cpp"
        "jleImage.cpp"
        "jleImageFlipped.cpp"
        "jleComponent.cpp"
        "jleGame.cpp"
        "jleObject.cpp"
        #"jleAseprite.cpp"
        "jleScene.cpp"
        # "jleTextRendering.cpp"
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
        #"jleQuadRendering.cpp"
        #"jleRendering.cpp"
        "jleDynamicLogAppender.cpp"
        "jleShader.cpp"
        "jleTexture.cpp"
        "jleWindow.cpp"
        "jleTransform.cpp"
        #"cSprite.cpp"
        #"cSpriteDepth.cpp"
        #"cAseprite.cpp"
        "cCamera.cpp"
        #"jleFont.cpp"
        #"cText.cpp"
        "jleTimerManager.cpp"
        #"cUITransformUpdater.cpp"
        "jleNetworking.cpp"
        "jleNetworkingNative.cpp"
        "jleNetworkingJS.cpp"
        "jleNetScene.cpp"
        #"jleSpritesheet.cpp"
        #"cSpritesheetDepth.cpp"
        #"cSpritesheet.cpp"
        "jlePath.cpp"
        "jleProfiler.cpp"
        "jleCamera.cpp"
        "jle3DRenderer.cpp"
        "jle3DRendererGraph.cpp"
        "cCameraFPV.cpp"
        "jleMesh.cpp"
        "cMesh.cpp"
        "jleSkybox.cpp"
        "cSkybox.cpp"
        "cLight.cpp"
        "cLightDirectional.cpp"
        "jleFramebufferScreen.cpp"
        "jleMaterial.cpp"
        "jleExplicitInclude.cpp"
        "jlePhysics.cpp"
        "jlePhysicsDebugDrawer.cpp"
        "cRigidbody.cpp"
        "cLuaScript.cpp"
        "jleLuaScript.cpp"
        "jleLuaScriptComponent.cpp"
        "jleFileChangeNotifier.cpp"
        "jleLuaEnvironment.cpp"
        "jleGLError.cpp"
        "jleInput.cpp")

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
            "editor/jleEditorWindowsPanel.cpp"
            "editor/jleEditorTextEdit.cpp"
            "editor/jleEditorResourceEdit.cpp"
            "editor/jleEditorNotifications.cpp"
            "editor/jleEditorGizmos.cpp"
            "editor/jleEditorSaveState.cpp"
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
endif ()

# TODO: Move Assimp to be editor only
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
if (BUILD_EMSCRIPTEN)
    set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
endif ()
add_subdirectory(3rdparty/git_submodules/assimp)
target_link_libraries(engine PUBLIC assimp)
target_include_directories(engine SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules/assimp/include")

target_link_libraries(engine PUBLIC zlib)

add_subdirectory(3rdparty/lua)
target_link_libraries(engine PUBLIC vanilla_lua)

if (NOT BUILD_EMSCRIPTEN) # Native build, excluding some Emscripten-embedded libraries else()

    target_link_libraries(engine PUBLIC ${CMAKE_DL_LIBS})

    # Compile glad.c only when building native
    target_sources(engine PRIVATE "3rdparty/glad/glad.cpp")

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

set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
add_subdirectory(3rdparty/git_submodules/bullet3)
target_link_libraries(engine PUBLIC BulletDynamics BulletCollision Bullet3Common BulletInverseDynamics BulletSoftBody LinearMath)
target_include_directories(engine SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules/bullet3/src")

# Disable HarfBuzz Dependency
set(FT_DISABLE_HARFBUZZ ON)
set(FT_REQUIRE_ZLIB ON)
add_subdirectory(3rdparty/freetype)
target_link_libraries(engine PUBLIC freetype)

set(FREETYPE_INCLUDE_DIRS 3rdparty/freetype/include)
set(FREETYPE_LIBRARY freetype)

add_definitions(-DRMLUI_STATIC_LIB)
set(BUILD_SAMPLES OFF)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(3rdparty/git_submodules/RmlUi)
if (WIN32)
    target_link_libraries(engine PRIVATE shlwapi imm32) # shlwapi and imm32 needed for "shell"
endif ()
target_include_directories(engine PUBLIC 3rdparty/git_submodules/RmlUi/Include RmlUiImplementation RmlUiImplementation/shell/include 3rdparty/lua)
target_link_libraries(engine PUBLIC RmlCore RmlDebugger)

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

if (BUILD_UNITY)
    set_target_properties(engine PROPERTIES
            UNITY_BUILD ON
            UNITY_BUILD_BATCH_SIZE 8)

    # Known compilation units to be problematic for unity builds are listed here.
    set_source_files_properties(
            "3rdparty/stb_image.cpp"
            "3rdparty/tiny_obj_loader.cpp"
            "cSkybox.cpp"
            "3rdparty/ImGui/imgui.cpp"
            "3rdparty/ImGui/imgui_draw.cpp"
            "3rdparty/ImGui/imgui_tables.cpp"
            "3rdparty/ImGui/imgui_widgets.cpp"
            "3rdparty/ImGui/ImGuizmo.cpp"
            "3rdparty/glad/glad.cpp"
            PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
endif ()

if (BUILD_PRECOMPILED_HEADER)
    target_precompile_headers(engine PRIVATE precompile.h)
endif ()