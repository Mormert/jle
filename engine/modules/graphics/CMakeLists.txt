add_library(jleGraphics STATIC
        "core/jleFrameBufferInterface.cpp"
        "core/jleFramebufferMultisample.cpp"
        "core/jleFramebufferShadowMap.cpp"
        "core/jleFramebufferShadowCubeMap.cpp"
        "core/jleFramebufferPicking.cpp"
        "core/jleFullscreenRendering.cpp"
        "core/jleFramebufferScreen.cpp"
        "core/jleGLError.cpp"
        "jleShader.cpp"
        "jleTexture.cpp"
        "jle3DRenderer.cpp"
        "jleFramePacket.cpp"
        "jleMesh.cpp"
        "jleSkybox.cpp"
        "jleMaterial.cpp"
        "jleTextureRefOrRGBA.cpp"
        "jleSkinnedMesh.cpp"
        "jleRenderThread.cpp"
        "runtime/components/cCamera.cpp"
        "runtime/components/cCameraFPV.cpp"
        "runtime/components/cMesh.cpp"
        "runtime/components/cSkinnedMesh.cpp"
        "runtime/components/cSkybox.cpp"
        "runtime/components/cLight.cpp"
        "runtime/components/cLightDirectional.cpp"
)

# TODO: It makes no sense to have the graphics module depend on headers from especially bullet here
# It is currently required since the jleMesh has bullet3 features built-in.
target_include_directories(jleGraphics PUBLIC SYSTEM "${JLE_3RDPARTY_DIRECTORY}/git_submodules/glfw/include")
target_include_directories(jleGraphics SYSTEM PUBLIC "${JLE_3RDPARTY_DIRECTORY}/git_submodules_forks/jle_bullet3/src")
target_include_directories(jleGraphics PUBLIC "${JLE_3RDPARTY_DIRECTORY}/git_submodules_forks/jle_RmlUi/Include" "RmlUiImplementation" "RmlUiImplementation/shell/include")


# TODO: Move Assimp to its own module for loading meshes?
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
add_subdirectory(${JLE_3RDPARTY_DIRECTORY}/git_submodules/assimp "${CMAKE_CURRENT_BINARY_DIR}/assimp")
target_link_libraries(jleGraphics PUBLIC assimp)
target_include_directories(jleGraphics SYSTEM PUBLIC "${JLE_3RDPARTY_DIRECTORY}/git_submodules/assimp/include")