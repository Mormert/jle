# Copyright (C) 2007-2013 LuaDist.
# Created by Peter Drahoš, David Manura
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

# Modified by Johan Lind (2024) for use in Jet-Lagged Engine.

project(luasocket C)
cmake_minimum_required(VERSION 2.8)
# Configuration

option(USE_LUA53 "Controls whether the library will be for Lua 5.3 or Ravi, default is OFF" OFF)
option(LUASOCKET_UNIX "Use only unix domain features." OFF)

SET(CMAKE_MODULE_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
        "${CMAKE_MODULE_PATH}")

#include(${CMAKE_INSTALL_PREFIX}/cmake/lua-config.cmake)

set(LUA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}../lua)
set(LUA_INCDIR ${CMAKE_CURRENT_SOURCE_DIR}../lua)
set(LUALIB vanilla_lua)

message(STATUS "LUA_INCLUDE_DIR: ${LUA_INCLUDE_DIR}")
message(STATUS "LUA_LIBRARIES  : ${LUA_LIBRARIES}")

message(STATUS "LUA_INCDIR     : ${LUA_INCDIR}")
message(STATUS "LUA_LIBDIR     : ${LUA_LIBDIR}")
message(STATUS "LUA_BINDIR     : ${LUA_BINDIR}")
message(STATUS "LUALIB         : ${LUALIB}")
message(STATUS "LIBDIR (LUA_CPATH)  : ${LIBDIR}")
message(STATUS "LUADIR (LUA_PATH)   : ${LUADIR}")

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")
INCLUDE_DIRECTORIES("${LUA_INCDIR}")
LINK_DIRECTORIES("${LUA_LIBDIR}")

# The standard luasocket build has this enabled in release mode.
# It is required for running the test suite.
add_definitions(-DLUASOCKET_DEBUG)
if (MINGW)
    add_definitions(-DLUASOCKET_INET_PTON -D_WIN32_WINNT=0x501)
endif ()

# Shared library exporting.
if (WIN32)
    add_definitions("-DMIME_API=__declspec(dllexport)" "-DLUASOCKET_API=__declspec(dllexport)")
endif ()

if (WIN32 AND NOT CYGWIN)
    set(SOCKETC src/wsocket.c)
else ()
    set(SOCKETC src/usocket.c)
endif ()

set(SRC_SOCKET src/luasocket.c src/timeout.c src/buffer.c src/io.c src/auxiliar.c
        src/options.c src/inet.c src/tcp.c src/udp.c src/except.c src/select.c ${SOCKETC})

set(UNIX_SOCKET src/unix.c src/io.c src/auxiliar.c src/timeout.c src/options.c
        src/buffer.c ${SOCKETC})

if (WIN32 AND NOT CYGWIN)
    set(LIB_SOCKET ws2_32)
endif ()

# Solaris needs specific links
if (CMAKE_SYSTEM_NAME STREQUAL "SunOS")
    set(LIB_SOCKET socket resolv rt)
endif ()

ADD_LIBRARY(socket_core SHARED ${SRC_SOCKET})
set_target_properties(socket_core PROPERTIES PREFIX "")
target_link_libraries(socket_core ${LIB_SOCKET} ${LUALIB})

ADD_LIBRARY(mime_core SHARED src/mime.c)
set_target_properties(mime_core PROPERTIES PREFIX "")
target_link_libraries(mime_core ${LIB_SOCKET} ${LUALIB})

if (LUASOCKET_UNIX)
    ADD_LIBRARY(socket_unix MODULE ${UNIX_SOCKET})
    set_target_properties(socket_unix PROPERTIES PREFIX "")
    target_link_libraries(socket_unix ${LIB_SOCKET} ${LUALIB})
endif ()

set_target_properties(socket_core PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
set_target_properties(mime_core PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# For both release and debug builds, copy and rename the DLLs accordingly
add_custom_command(TARGET socket_core POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:socket_core> "${JLE_ENGINE_PATH}/EngineResources/scripts/3rdparty/socket_core.dll"
    COMMENT "Copying socket_core DLL..."
)

add_custom_command(TARGET mime_core POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:mime_core> "${JLE_ENGINE_PATH}/EngineResources/scripts/3rdparty/mime_core.dll"
    COMMENT "Copying mime_core DLL..."
)

# Copy Lua files
file(COPY
    src/ltn12.lua
    src/socket.lua
    src/mime.lua
    src/http.lua
    src/url.lua
    src/tp.lua
    src/ftp.lua
    src/headers.lua
    src/smtp.lua
    DESTINATION "${JLE_ENGINE_PATH}/EngineResources/scripts/3rdparty"
)

