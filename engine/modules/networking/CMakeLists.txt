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

include(FetchContent)

FetchContent_Declare(
        enet
        GIT_REPOSITORY https://github.com/zpl-c/enet.git
        GIT_TAG        42c03e1ff26bf5873a9de8c52645ffd3ac9687e4
)

set(ENET_STATIC ON)

FetchContent_MakeAvailable(enet)

target_include_directories(jleNetworking PUBLIC "${enet_SOURCE_DIR}/include")
target_link_libraries(jleNetworking PUBLIC enet_static)
