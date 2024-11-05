add_library(jleCore STATIC
        "../3rdparty/stb_image.cpp"
        "../3rdparty/stb_image_write.cpp"
        "jleResourceHolder.cpp"
        "jleFileWatcher.cpp"
        "jleResourceInterface.cpp"
        "jleTimerManager.cpp"
        "jlePath.cpp"
        "jleProfiler.cpp"
        "jleTransform.cpp"
        "jleCommandArguments.cpp"
        "jleDynamicLogAppender.cpp"
        "jleCamera.cpp"
        "jleSceneNetworked.cpp"
        "jleSceneServer.cpp"
        "jleSceneClient.cpp"
        "jleNew.cpp"
        "jleImage.cpp"
        "jleImageFlipped.cpp"
        "jleResourceIndexer.cpp"
        "jleComponent.cpp"
        "jleObject.cpp"
        "jleScene.cpp"
)

add_subdirectory(jlECS)
target_link_libraries(jleCore PUBLIC jlECS)

add_subdirectory(${JLE_3RDPARTY_DIRECTORY}/git_submodules_forks/jle_EASTL "${CMAKE_CURRENT_BINARY_DIR}/eastl")
target_include_directories(jleCore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/jleSTL")
target_link_libraries(jleCore PUBLIC EASTL)

set(ENET_STATIC ON)
add_subdirectory(${JLE_3RDPARTY_DIRECTORY}/git_submodules/enet "${CMAKE_CURRENT_BINARY_DIR}/enet")
target_include_directories(jleCore PUBLIC 3rdparty/git_submodules/enet/include)
target_link_libraries(jleCore PUBLIC enet_static)


# Todo: move to its own physics module

set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
add_subdirectory(${JLE_3RDPARTY_DIRECTORY}/git_submodules_forks/jle_bullet3 "${CMAKE_CURRENT_BINARY_DIR}/bullet")
target_link_libraries(jleCore PUBLIC BulletDynamics BulletCollision Bullet3Common BulletInverseDynamics BulletSoftBody LinearMath)
target_include_directories(jleCore SYSTEM PUBLIC "${JLE_3RDPARTY_DIRECTORY}/git_submodules_forks/jle_bullet3/src")