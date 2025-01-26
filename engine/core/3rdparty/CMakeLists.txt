
include(FetchContent)
include(ExternalProject)

# EASTL / jleSTL
file(GLOB_RECURSE JLE_STL_HEADERS "core/jleSTL/**.h")

FetchContent_Declare(
        lz4
        GIT_REPOSITORY https://github.com/lz4/lz4.git
        GIT_TAG        v1.10.0
)

FetchContent_Populate(lz4)

set(LZ4_SOURCE_DIR ${lz4_SOURCE_DIR}/lib)

set(LZ4_SOURCES
        "${LZ4_SOURCE_DIR}/lz4.c"
        "${LZ4_SOURCE_DIR}/lz4hc.c"
        "${LZ4_SOURCE_DIR}/lz4file.c"
        "${LZ4_SOURCE_DIR}/lz4frame.c"
        "${LZ4_SOURCE_DIR}/xxhash.c"
)

set_source_files_properties(${LZ4_SOURCES} PROPERTIES LANGUAGE CXX)

add_library(jleCore_3rdparty STATIC
        # stb libraries
        "stb/stb_image.cpp"
        "stb/stb_image_write.cpp"

        # tiny obj loader
        "tinyobjloader/tiny_obj_loader.cpp"

        # WickedEngine
        "WickedEngine/wiJobSystem.cpp"

        # LZ4
        ${LZ4_SOURCES}

        # EASTL / jleSTL
        ${JLE_STL_HEADERS}
)

# ZLib
message("zlib bin")
message(${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(zlib)
set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
set(ZLIB_LIBRARY zlib)
set(ZLIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/zlib)
set(SKIP_INSTALL_ALL ON)
message(${zlib_SOURCE_DIR})
message(${zlib_BINARY_DIR})
include_directories(SYSTEM ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
target_include_directories(jleCore_3rdparty PUBLIC ${zlib_BINARY_DIR})

find_package(ZLIB REQUIRED)
target_link_libraries(jleCore_3rdparty PUBLIC zlib)

FetchContent_Declare(
        libzip
        GIT_REPOSITORY https://github.com/nih-at/libzip.git
        GIT_TAG        v1.11.3
)

# libzip & libzippp
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(BUILD_REGRESS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_DOC OFF CACHE BOOL "" FORCE)
set(LIBZIP_DO_INSTALL OFF CACHE BOOL "" FORCE)
set(SHARED_LIB_VERSIONNING OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(libzip)

FetchContent_Declare(
        libzippp
        GIT_REPOSITORY https://github.com/ctabin/libzippp.git
        GIT_TAG        libzippp-v7.1-1.10.1
)

FetchContent_Populate(libzippp)

add_library(libzippp STATIC "${libzippp_SOURCE_DIR}/src/libzippp.cpp")

target_include_directories(libzippp PUBLIC "${libzippp_SOURCE_DIR}/src")
target_link_libraries(libzippp PRIVATE zip)

target_link_libraries(jleCore_3rdparty PUBLIC libzippp)
target_include_directories(jleCore_3rdparty SYSTEM PUBLIC "${libzippp_SOURCE_DIR}/src")

# LZ4
target_include_directories(jleCore_3rdparty SYSTEM PUBLIC "${lz4_SOURCE_DIR}/lib")

# FreeType
set(FT_DISABLE_HARFBUZZ ON)
set(FT_REQUIRE_ZLIB ON)
add_subdirectory(freetype)
# set(FREETYPE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/freetype/include")
set(FREETYPE_INCLUDE_DIRS freetype/include)
set(FREETYPE_LIBRARY freetype)
target_link_libraries(jleCore_3rdparty PUBLIC freetype)



set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_3MF_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_3MF_IMPORTER OFF CACHE BOOL "" FORCE)

if (JLE_BUILD_EMSCRIPTEN)
    set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
endif()

FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG        v5.4.3
)

FetchContent_MakeAvailable(assimp)

target_link_libraries(jleCore_3rdparty PUBLIC assimp)

# Fetch Tracy
FetchContent_Declare(
        tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG        v0.11.0
)

set(TRACY_ENABLE ON)
set(TRACY_ON_DEMAND ON)

FetchContent_MakeAvailable(tracy)
target_link_libraries(jleCore_3rdparty PUBLIC Tracy::TracyClient)

set(TRACY_PROFILER_DIR "${tracy_SOURCE_DIR}/profiler")
target_compile_definitions(jleCore_3rdparty PUBLIC JLE_TRACY_PROFILER_PATH="${TRACY_PROFILER_DIR}")

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        0.9.9.8
)

FetchContent_MakeAvailable(glm)

FetchContent_Declare(
        plog
        GIT_REPOSITORY https://github.com/SergiusTheBest/plog.git
        GIT_TAG        1.1.10
)

FetchContent_MakeAvailable(plog)

FetchContent_Declare(
        jle_EASTL
        GIT_REPOSITORY https://github.com/Mormert/jle_EASTL.git
        GIT_TAG        master
)

FetchContent_MakeAvailable(jle_EASTL)


# Tracy, Assimp, EASTL / jleSTL, glm, plog
target_include_directories(jleCore_3rdparty PUBLIC SYSTEM
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${tracy_SOURCE_DIR}/public/tracy"
    "${jle_EASTL_SOURCE_DIR}/include"
    "${jle_EASTL_SOURCE_DIR}/jle_EABase/include/Common"
    "${plog_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lua"
    "${CMAKE_CURRENT_SOURCE_DIR}/lua/lua"
    "${assimp_SOURCE_DIR}/include"
    "${glm_SOURCE_DIR}"
)

if (MSVC)
    add_subdirectory(detours)
    target_link_libraries(jleCore_3rdparty PRIVATE lib_detours)
endif ()