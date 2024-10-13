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

set(JLE_CORE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/core")

set(JLE_CORE_3RD_PARTY_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty"
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules"
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules/plog/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules/tracy/public/tracy"
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules_forks/jle_EASTL/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules_forks/jle_EASTL/jle_EABase/include/Common"
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules/glm"
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/lua")

set(JLE_3RDPARTY_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty")

include_directories(${JLE_CORE_3RD_PARTY_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

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

file(GLOB_RECURSE JLE_STL_HEADERS "core/jleSTL/**.h")

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
        "modules/graphics/RmlUiImplementation/RmlUi_Backend_GLFW_GL3.cpp"
        "modules/graphics/RmlUiImplementation/RmlUi_Renderer_GL3.cpp"
        "modules/graphics/RmlUiImplementation/RmlUi_Platform_GLFW.cpp"
        "modules/graphics/RmlUiImplementation/shell/src/PlatformExtensions.cpp"
        "modules/graphics/RmlUiImplementation/shell/src/RendererExtensions.cpp"
        "modules/graphics/RmlUiImplementation/shell/src/Shell.cpp"
        "modules/graphics/RmlUiImplementation/shell/src/ShellFileInterface.cpp"
        "modules/jleEngineModulesContext.cpp"
        "modules/jleEditorModulesContext.cpp"
        "modules/game/jleGameRuntime.cpp"
        "modules/game/jleGame.cpp"
        "modules/networking/jleNetworkEvent.cpp"
        "modules/networking/jleNetworkEventOutQueue.cpp"
        "jleGameEngine.cpp"
        "modules/physics/jlePhysics.cpp"
        "modules/physics/jlePhysicsDebugDrawer.cpp"
        "modules/physics/components/cRigidbody.cpp"
        "modules/animation/components/cAnimator.cpp"
        "modules/animation/jleAnimationBone.cpp"
        "modules/animation/jleAnimation.cpp"
        "modules/networking/components/cTransformNetSync.cpp"
        "modules/animation/jleAnimationFinalMatrices.cpp"
        )

add_subdirectory(core)
target_link_libraries(engine PUBLIC jleCore)

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

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



target_link_libraries(engine PUBLIC zlib)

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
target_include_directories(engine PUBLIC 3rdparty/git_submodules_forks/jle_RmlUi/Include modules/graphics/RmlUiImplementation modules/graphics/RmlUiImplementation/shell/include)
target_link_libraries(engine PUBLIC RmlCore RmlDebugger)

add_subdirectory(modules)
target_link_libraries(engine PUBLIC jleModules)

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