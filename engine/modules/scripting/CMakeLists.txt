add_library(jleScripting STATIC
        jleLuaScript.cpp
        jleLuaEnvironment.cpp
        jleLuaClass.cpp
        components/cLuaScript.cpp
)

target_include_directories(jleScripting PRIVATE SYSTEM 3rdparty)

add_subdirectory(../../core/3rdparty/lua/lua ${CMAKE_CURRENT_BINARY_DIR}/lua)
target_link_libraries(jleScripting PRIVATE vanilla_lua jleInput jleCore)

if (JLE_BUILD_EDITOR AND WIN32)
    add_subdirectory(3rdparty/luasocket)
    add_dependencies(jleScripting socket_core)
    add_dependencies(jleScripting mime_core)
endif ()
