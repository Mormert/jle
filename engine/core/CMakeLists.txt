add_library(jleCore STATIC
        "src/jlECSSaveLoad.cpp"
        "src/jleResourceHolder.cpp"
        "src/jleFileWatcher.cpp"
        "src/jleResourceInterface.cpp"
        "src/jleTimerManager.cpp"
        "src/jlePath.cpp"
        "src/jleProfiler.cpp"
        "src/jleTransform.cpp"
        "src/jleCommandArguments.cpp"
        "src/jleDynamicLogAppender.cpp"
        "src/jleCamera.cpp"
        "src/jleNew.cpp"
        "src/jleImage.cpp"
        "src/jleImageFlipped.cpp"
        "src/jleComponent.cpp"
        "src/jleObject.cpp"
        "src/jleScene.cpp"
)

add_subdirectory(3rdparty)
target_link_libraries(jleCore PUBLIC jleCore_3rdparty)

add_subdirectory(jlECS)
target_link_libraries(jleCore PUBLIC jlECS)

target_include_directories(jleCore PUBLIC SYSTEM
        "${CMAKE_CURRENT_SOURCE_DIR}/public"
)

target_include_directories(jleCore PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/public/core"
)

target_include_directories(jleCore PUBLIC SYSTEM
        "${CMAKE_CURRENT_SOURCE_DIR}/jleSTL"
)