add_library(jleScripting STATIC
        jleLuaScript.cpp
        jleLuaEnvironment.cpp
        jleLuaClass.cpp
        components/cLuaScript.cpp
)

add_subdirectory(${JLE_3RDPARTY_DIRECTORY}/lua "${CMAKE_CURRENT_BINARY_DIR}/lua")
target_link_libraries(jleScripting PUBLIC vanilla_lua jleInput)
target_link_libraries(engine PUBLIC vanilla_lua)

if (JLE_BUILD_EDITOR AND WIN32)
    add_subdirectory(${JLE_3RDPARTY_DIRECTORY}/luasocket "${CMAKE_CURRENT_BINARY_DIR}/luasocket")
    add_dependencies(jleScripting socket_core)
    add_dependencies(jleScripting mime_core)
endif ()
