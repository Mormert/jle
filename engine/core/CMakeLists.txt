add_library(jleCore STATIC
        "jleResource.cpp"
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

add_subdirectory(${JLE_3RDPARTY_DIRECTORY}/git_submodules_forks/jle_EASTL "${CMAKE_CURRENT_BINARY_DIR}/eastl")
target_include_directories(jleCore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/jleSTL")
target_link_libraries(jleCore PUBLIC EASTL)

set(ENET_STATIC ON)
add_subdirectory(${JLE_3RDPARTY_DIRECTORY}/git_submodules/enet "${CMAKE_CURRENT_BINARY_DIR}/enet")
target_include_directories(jleCore PUBLIC 3rdparty/git_submodules/enet/include)
target_link_libraries(jleCore PUBLIC enet_static)