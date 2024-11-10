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




# Todo: move to its own physics module

