
if(BUILD_EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set_target_properties(${JLE_GAME_BUILD} PROPERTIES OUTPUT_NAME "index")
endif()

FILE(COPY GameResources DESTINATION ../${JLE_GAME_BUILD})