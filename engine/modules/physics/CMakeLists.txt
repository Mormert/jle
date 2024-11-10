add_library(jlePhysics STATIC
    "jlePhysics.cpp"
    "jlePhysicsDebugDrawer.cpp"
    "jlePhysicsModule.cpp"
    "components/cRigidbody.cpp"
)

set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
add_subdirectory(3rdparty/jle_bullet3)
target_link_libraries(jlePhysics PUBLIC BulletDynamics BulletCollision Bullet3Common BulletInverseDynamics BulletSoftBody LinearMath)

target_include_directories(jlePhysics SYSTEM PUBLIC "3rdparty/jle_bullet3/src")

# TODO: only jlePhysics should have include dir access here?
target_include_directories(jleGraphics SYSTEM PUBLIC "3rdparty/jle_bullet3/src")