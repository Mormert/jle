
add_subdirectory(hierarchy)
add_subdirectory(animation)
add_subdirectory(scripting)
add_subdirectory(windowing)
add_subdirectory(input)
add_subdirectory(mesh)
add_subdirectory(image)
add_subdirectory(graphics)
add_subdirectory(physics)
add_subdirectory(networking)
add_subdirectory(audio)

target_link_libraries(jleHierarchyModule PRIVATE jleCore)
target_link_libraries(jleAnimation PRIVATE jleCore)
target_link_libraries(jleScripting PRIVATE jleCore)
target_link_libraries(jleWindowing PRIVATE jleCore)
target_link_libraries(jleInput PRIVATE jleCore)
target_link_libraries(jleMeshModule PRIVATE jleCore)
target_link_libraries(jleImageModule PRIVATE jleCore)
target_link_libraries(jlePhysics PRIVATE jleCore)
target_link_libraries(jleGraphics PRIVATE jleCore jleWindowing)
target_link_libraries(jleNetworking PRIVATE jleCore)
target_link_libraries(jleAudio PRIVATE jleCore)

target_link_libraries(engine PUBLIC
        jleHierarchyModule
        jleAnimation
        jleScripting
        jleWindowing
        jleMeshModule
        jleImageModule
        jleGraphics
        jlePhysics
        jleNetworking
        jleAudio
)
