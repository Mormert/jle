add_library(jleGraphics STATIC
        "core/jleFrameBufferInterface.cpp"
        "core/jleFramebufferMultisample.cpp"
        "core/jleFramebufferShadowMap.cpp"
        "core/jleFramebufferShadowCubeMap.cpp"
        "core/jleFramebufferPicking.cpp"
        "core/jleFullscreenRendering.cpp"
        "core/jleFramebufferScreen.cpp"
        "core/jleGLError.cpp"
        "jleGraphicsModule.cpp"
        "jleShader.cpp"
        "jleTexture.cpp"
        "jleGraphics.cpp"
        "jleFramePacket.cpp"
        "jleSkybox.cpp"
        "jleMaterial.cpp"
        "jleTextureRefOrRGBA.cpp"
        "jleSkinnedMesh.cpp"
        "jleRenderThread.cpp"
        "runtime/components/cCamera.cpp"
        "runtime/components/cMeshRenderer.cpp"
        "runtime/components/cSkinnedMesh.cpp"
        "runtime/components/cSkybox.cpp"
        "runtime/components/cLight.cpp"
        "runtime/components/cLightDirectional.cpp"

        "RmlUiImplementation/RmlUi_Backend_GLFW_GL3.cpp"
        "RmlUiImplementation/RmlUi_Renderer_GL3.cpp"
        "RmlUiImplementation/RmlUi_Platform_GLFW.cpp"
        "RmlUiImplementation/shell/src/PlatformExtensions.cpp"
        "RmlUiImplementation/shell/src/RendererExtensions.cpp"
        "RmlUiImplementation/shell/src/Shell.cpp"
        "RmlUiImplementation/shell/src/ShellFileInterface.cpp"
)

if (NOT JLE_BUILD_EMSCRIPTEN) # Native build, excluding some Emscripten-embedded libraries
    # Compile glad.c only when building native
    target_sources(jleGraphics PRIVATE "3rdparty/glad/glad.cpp")

    target_link_libraries(jleGraphics PRIVATE ${CMAKE_DL_LIBS})
endif ()

add_definitions(-DRMLUI_STATIC_LIB)
set(BUILD_SAMPLES OFF)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set(FREETYPE_INCLUDE_DIRS ../../core/3rdparty/freetype)
set(FREETYPE_LIBRARY freetype)

include(FetchContent)

FetchContent_Declare(
        jle_RmlUi
        GIT_REPOSITORY https://github.com/Mormert/jle_RmlUi.git
        GIT_TAG        master
)

FetchContent_MakeAvailable(jle_RmlUi)

target_include_directories(jleGraphics PUBLIC
        "${jle_RmlUi_SOURCE_DIR}/Include"
        "${CMAKE_CURRENT_SOURCE_DIR}/RmlUiImplementation"
        "${CMAKE_CURRENT_SOURCE_DIR}/RmlUiImplementation/shell/include"
)

target_link_libraries(jleGraphics PUBLIC RmlCore RmlDebugger)
if (WIN32)
    target_link_libraries(jleGraphics PRIVATE shlwapi imm32) # shlwapi and imm32 needed for "shell"
endif ()
