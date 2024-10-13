
function(add_core_dependency target)
    target_include_directories(${target} PUBLIC ${JLE_CORE_3RD_PARTY_DIRS})
    target_include_directories(${target} PUBLIC ${JLE_CORE_INCLUDE_DIR})
    target_include_directories(${target} PUBLIC SYSTEM ${JLE_CORE_INCLUDE_DIR}/jleSTL)
endfunction()

message("The core include path is ${JLE_CORE_INCLUDE_DIR}")

add_subdirectory(scripting)
add_subdirectory(windowing)
add_subdirectory(input)
add_subdirectory(graphics)

add_library(jleModules STATIC
    jleModules.cpp)

add_core_dependency(jleScripting)
add_core_dependency(jleWindowing)
add_core_dependency(jleInput)
add_core_dependency(jleGraphics)

target_link_libraries(engine PUBLIC jleScripting jleWindowing jleGraphics)

