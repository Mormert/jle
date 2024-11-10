add_library(jleNetworking STATIC
        "jleNetworkingModule.cpp"
        "jleNetworkEvent.cpp"
        "jleNetworkEventOutQueue.cpp"
        "jleNetworkingComponent.cpp"
        "jleSceneNetworked.cpp"
        "jleSceneServer.cpp"
        "jleSceneClient.cpp"
        "components/cTransformNetSync.cpp"
)

set(ENET_STATIC ON)
add_subdirectory(3rdparty/enet)
target_include_directories(jleNetworking PUBLIC 3rdparty/enet/include)
target_link_libraries(jleNetworking PUBLIC enet_static)