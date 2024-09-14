#############################################################################################
#                                                                                           #
#               ,     .     ,                      .   ,--.                                 #
#               |     |     |                      |   |            o                       #
#               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               #
#               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               #
#              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               #
#                                                                                           #
#     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     #
#          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       #
#     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     #
#                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 #
#                                                                                           #
#############################################################################################

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

file(GLOB_RECURSE JLE_STL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/jleSTL/**.h")

set(LZ4_SOURCES
        "3rdparty/git_submodules/lz4/lib/lz4.c"
        "3rdparty/git_submodules/lz4/lib/lz4hc.c"
        "3rdparty/git_submodules/lz4/lib/lz4file.c"
        "3rdparty/git_submodules/lz4/lib/lz4frame.c"
        "3rdparty/git_submodules/lz4/lib/xxhash.c"
)

set_source_files_properties(${LZ4_SOURCES} PROPERTIES LANGUAGE CXX)

add_library(engine STATIC
        ${JLE_STL_HEADERS}
        ${LZ4_SOURCES}
        "3rdparty/stb_image.cpp"
        "3rdparty/stb_image_write.cpp"
        "3rdparty/tiny_obj_loader.cpp"
        "3rdparty/WickedEngine/wiJobSystem.cpp"
        "RmlUiImplementation/RmlUi_Backend_GLFW_GL3.cpp"
        "RmlUiImplementation/RmlUi_Renderer_GL3.cpp"
        "RmlUiImplementation/RmlUi_Platform_GLFW.cpp"
        "RmlUiImplementation/shell/src/PlatformExtensions.cpp"
        "RmlUiImplementation/shell/src/RendererExtensions.cpp"
        "RmlUiImplementation/shell/src/Shell.cpp"
        "RmlUiImplementation/shell/src/ShellFileInterface.cpp"
        "modules/jleEngineModulesContext.cpp"
        "modules/jleEditorModulesContext.cpp"
        "modules/game/jleGameRuntime.cpp"
        "core/jleResourceIndexer.cpp"
        "jleNew.cpp"
        "jleImage.cpp"
        "jleImageFlipped.cpp"
        "jleComponent.cpp"
        "jleGame.cpp"
        "jleObject.cpp"
        "jleScene.cpp"
        "jleNetworkEvent.cpp"
        "jleNetworkEventOutQueue.cpp"
        "jleSceneNetworked.cpp"
        "jleSceneServer.cpp"
        "jleSceneClient.cpp"
        "jleGameEngine.cpp"
        "modules/input/hardware/jleKeyboardInput.cpp"
        "modules/input/hardware/jleMouseInput.cpp"
        "modules/graphics/core/jleFrameBufferInterface.cpp"
        "modules/graphics/core/jleFramebufferMultisample.cpp"
        "modules/graphics/core/jleFramebufferShadowMap.cpp"
        "modules/graphics/core/jleFramebufferShadowCubeMap.cpp"
        "modules/graphics/core/jleFramebufferPicking.cpp"
        "modules/graphics/core/jleFullscreenRendering.cpp"
        "jleDynamicLogAppender.cpp"
        "jleShader.cpp"
        "core/jleCommandArguments.cpp"
        "jleTexture.cpp"
        "modules/windowing/jleWindow.cpp"
        "jleTransform.cpp"
        "modules/graphics/runtime/components/cCamera.cpp"
        "modules/animation/components/cAnimator.cpp"
        "jleTimerManager.cpp"
        "jlePath.cpp"
        "jleProfiler.cpp"
        "jleCamera.cpp"
        "modules/graphics/jle3DRenderer.cpp"
        "modules/graphics/jleFramePacket.cpp"
        "modules/graphics/runtime/components/cCameraFPV.cpp"
        "jleLuaClass.cpp"
        "jleMesh.cpp"
        "modules/graphics/runtime/components/cMesh.cpp"
        "jleSkybox.cpp"
        "modules/graphics/runtime/components/cSkybox.cpp"
        "modules/graphics/runtime/components/cLight.cpp"
        "modules/graphics/runtime/components/cLightDirectional.cpp"
        "modules/graphics/core/jleFramebufferScreen.cpp"
        "jleMaterial.cpp"
        "jlePhysics.cpp"
        "jlePhysicsDebugDrawer.cpp"
        "modules/physics/components/cRigidbody.cpp"
        "modules/scripting/components/cLuaScript.cpp"
        "jleLuaScript.cpp"
        "core/jleFileWatcher.cpp"
        "jleLuaEnvironment.cpp"
        "modules/graphics/core/jleGLError.cpp"
        "modules/input/jleInput.cpp"
        "jleTextureRefOrRGBA.cpp"
        "jleSkinnedMesh.cpp"
        "modules/animation/jleAnimationBone.cpp"
        "modules/animation/jleAnimation.cpp"
        "modules/graphics/runtime/components/cSkinnedMesh.cpp"
        "modules/networking/components/cTransformNetSync.cpp"
        "modules/animation/jleAnimationFinalMatrices.cpp"
        "jleResource.cpp"
        "jleResourceInterface.cpp"
        "jleRenderThread.cpp")

if (JLE_BUILD_EDITOR)
    target_sources(engine PRIVATE
            "editor/jleEditor.cpp"
            "editor/jleImGuiCerealArchive.cpp"
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
            "editor/jleImGuiExtensions.cpp"
            "editor/jleEditor3DImportWindow.cpp"
            "editor/jleEditorFrameGraphWindow.cpp"
            "editor/jleEditorBuild.cpp"
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
            "3rdparty/ImGui/ImGuizmo.cpp"
            "3rdparty/implot/implot.cpp"
            "3rdparty/implot/implot_items.cpp")

    if(APPLE)
        target_sources(engine PRIVATE "editor/jleMacOSWindow.mm")
    endif ()

    add_definitions(-DTRACY_ENABLE)
    add_definitions(-DTRACY_ON_DEMAND)
    add_definitions(-DRMLUI_ENABLE_PROFILING)
    add_subdirectory(3rdparty/git_submodules/tracy)
    target_link_libraries(engine PUBLIC Tracy::TracyClient)
endif ()

# TODO: Move Assimp to be editor only
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
# Exclude 3MF since it uses zip_open which caused name conflict with libzip
set(ASSIMP_BUILD_3MF_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_3MF_IMPORTER OFF CACHE BOOL "" FORCE)
if (JLE_BUILD_EMSCRIPTEN)
    set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
endif ()
add_subdirectory(3rdparty/git_submodules/assimp)
target_link_libraries(engine PUBLIC assimp)
target_include_directories(engine SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules/assimp/include")

target_link_libraries(engine PUBLIC zlib)

add_subdirectory(3rdparty/lua)
target_link_libraries(engine PUBLIC vanilla_lua)

if (JLE_BUILD_EDITOR AND WIN32)
    add_subdirectory(3rdparty/luasocket)
    add_dependencies(engine socket_core)
    add_dependencies(engine mime_core)
endif ()

if (NOT JLE_BUILD_EMSCRIPTEN) # Native build, excluding some Emscripten-embedded libraries else()

    target_link_libraries(engine PUBLIC ${CMAKE_DL_LIBS})

    # Compile glad.c only when building native
    target_sources(engine PRIVATE "3rdparty/glad/glad.cpp")

    # Remove GLFW tests, examples and docs from build
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory(3rdparty/git_submodules/glfw)
    target_link_libraries(engine PUBLIC glfw)

endif ()

set(ENET_STATIC ON)
add_subdirectory(3rdparty/git_submodules/enet)
target_include_directories(engine PUBLIC 3rdparty/git_submodules/enet/include)
target_link_libraries(engine PUBLIC enet_static)

add_subdirectory(3rdparty/SoLoud)
target_link_libraries(engine PUBLIC SoLoud)

set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
add_subdirectory(3rdparty/git_submodules_forks/jle_bullet3)
target_link_libraries(engine PUBLIC BulletDynamics BulletCollision Bullet3Common BulletInverseDynamics BulletSoftBody LinearMath)
target_include_directories(engine SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules_forks/jle_bullet3/src")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(BUILD_REGRESS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_DOC OFF CACHE BOOL "" FORCE)
set(LIBZIP_DO_INSTALL OFF CACHE BOOL "" FORCE)
set(SHARED_LIB_VERSIONNING OFF CACHE BOOL "" FORCE)
add_subdirectory(3rdparty/git_submodules/libzip)

add_library(libzippp "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules/libzippp/src/libzippp.cpp")
target_link_libraries(libzippp PRIVATE libzip::zip)

target_link_libraries(engine PUBLIC libzippp)
target_include_directories(engine SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules/libzippp/src/")

target_include_directories(engine SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules/lz4/lib")

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
add_subdirectory(3rdparty/git_submodules_forks/jle_RmlUi)
if (WIN32)
    target_link_libraries(engine PRIVATE shlwapi imm32) # shlwapi and imm32 needed for "shell"
endif ()
target_include_directories(engine PUBLIC 3rdparty/git_submodules_forks/jle_RmlUi/Include RmlUiImplementation RmlUiImplementation/shell/include 3rdparty/lua)
target_link_libraries(engine PUBLIC RmlCore RmlDebugger)

add_subdirectory(3rdparty/git_submodules_forks/jle_EASTL)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/jleSTL")
target_link_libraries(engine PUBLIC EASTL)

target_include_directories(engine SYSTEM PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty"
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules/plog/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules/tracy/public/tracy"
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules_forks/jle_EASTL/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules_forks/jle_EASTL/jle_EABase/include/Common"
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules/glm")

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (JLE_BUILD_EMSCRIPTEN)
    # When building with Emscripten, it needs to find the pre-loaded assets
    # at the same location as the 'binary'
    FILE(COPY EditorResources DESTINATION ${PROJECT_BINARY_DIR})
    FILE(COPY EngineResources DESTINATION ${PROJECT_BINARY_DIR})
else ()
    FILE(COPY EditorResources DESTINATION ${PROJECT_BINARY_DIR}/${JLE_GAME_BUILD})
    FILE(COPY EngineResources DESTINATION ${PROJECT_BINARY_DIR}/${JLE_GAME_BUILD})
endif ()

if (JLE_BUILD_UNITY)
    set_target_properties(engine PROPERTIES
            UNITY_BUILD ON
            UNITY_BUILD_BATCH_SIZE 8)

    # Known compilation units to be problematic for unity builds are listed here.
    set_source_files_properties(
            "3rdparty/stb_image.cpp"
            "3rdparty/tiny_obj_loader.cpp"
            "modules/graphics/runtime/components/cSkybox.cpp"
            "3rdparty/ImGui/imgui.cpp"
            "3rdparty/ImGui/imgui_draw.cpp"
            "3rdparty/ImGui/imgui_tables.cpp"
            "3rdparty/ImGui/imgui_widgets.cpp"
            "3rdparty/ImGui/ImGuizmo.cpp"
            "3rdparty/glad/glad.cpp"
            PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)

    if(JLE_BUILD_EDITOR)
        if(APPLE)
            set_source_files_properties(
                    "editor/jleMacOSWindow.mm"
                    PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
        endif ()
    endif ()
endif ()

if (JLE_BUILD_PRECOMPILED_HEADER)
    target_precompile_headers(engine PRIVATE precompile.h)
endif ()