add_library(jlePhysics STATIC
    "jlePhysics.cpp"
    "jlePhysicsDebugDrawer.cpp"
    "jlePhysicsModule.cpp"
    "components/cRigidbody.cpp"
)

include(FetchContent)

FetchContent_Declare(
        jle_bullet3
        GIT_REPOSITORY https://github.com/Mormert/jle_bullet3.git
        GIT_TAG        master
)

set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(jle_bullet3)

target_link_libraries(jlePhysics PUBLIC BulletDynamics BulletCollision Bullet3Common BulletInverseDynamics BulletSoftBody LinearMath)

target_include_directories(jlePhysics SYSTEM PUBLIC "${jle_bullet3_SOURCE_DIR}/src")

# TODO: only jlePhysics should have include dir access here?
target_include_directories(jleGraphics SYSTEM PUBLIC "${jle_bullet3_SOURCE_DIR}/src")