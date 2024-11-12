
message("The core include path is ${JLE_CORE_INCLUDE_DIR}")

add_subdirectory(scripting)
add_subdirectory(windowing)
add_subdirectory(input)
add_subdirectory(graphics)
add_subdirectory(physics)
add_subdirectory(networking)

target_link_libraries(jleScripting PRIVATE jleCore)
target_link_libraries(jleWindowing PRIVATE jleCore)
target_link_libraries(jleInput PRIVATE jleCore)
target_link_libraries(jleGraphics PRIVATE jleCore)
target_link_libraries(jlePhysics PRIVATE jleCore)
target_link_libraries(jleNetworking PRIVATE jleCore)

target_link_libraries(engine PUBLIC jleScripting jleWindowing jleGraphics jlePhysics jleNetworking)

