
# EASTL / jleSTL
file(GLOB_RECURSE JLE_STL_HEADERS "core/jleSTL/**.h")

# LZ4
set(LZ4_SOURCES
        "git_submodules/lz4/lib/lz4.c"
        "git_submodules/lz4/lib/lz4hc.c"
        "git_submodules/lz4/lib/lz4file.c"
        "git_submodules/lz4/lib/lz4frame.c"
        "git_submodules/lz4/lib/xxhash.c"
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

# libzip & libzippp
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(BUILD_REGRESS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_DOC OFF CACHE BOOL "" FORCE)
set(LIBZIP_DO_INSTALL OFF CACHE BOOL "" FORCE)
set(SHARED_LIB_VERSIONNING OFF CACHE BOOL "" FORCE)
add_subdirectory(git_submodules/libzip)

add_library(libzippp "${CMAKE_CURRENT_SOURCE_DIR}/git_submodules/libzippp/src/libzippp.cpp")
target_link_libraries(libzippp PRIVATE libzip::zip)
target_link_libraries(jleCore_3rdparty PUBLIC libzippp)
target_include_directories(jleCore_3rdparty SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/git_submodules/libzippp/src/")

# LZ4
target_include_directories(jleCore_3rdparty SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/git_submodules/lz4/lib")


# FreeType
set(FT_DISABLE_HARFBUZZ ON)
set(FT_REQUIRE_ZLIB ON)
add_subdirectory(freetype)
# set(FREETYPE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/freetype/include")
set(FREETYPE_INCLUDE_DIRS freetype/include)
set(FREETYPE_LIBRARY freetype)
target_link_libraries(jleCore_3rdparty PUBLIC freetype)


# Assimp
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
# Exclude 3MF since it uses zip_open which caused name conflict with libzip
set(ASSIMP_BUILD_3MF_EXPORTER OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_3MF_IMPORTER OFF CACHE BOOL "" FORCE)
if (JLE_BUILD_EMSCRIPTEN)
    set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
endif ()
add_subdirectory(git_submodules/assimp)
target_link_libraries(jleCore_3rdparty PUBLIC assimp)

# Tracy
add_definitions(-DTRACY_ENABLE)
add_definitions(-DTRACY_ON_DEMAND)
add_definitions(-DRMLUI_ENABLE_PROFILING)
add_subdirectory(git_submodules/tracy)
target_link_libraries(jleCore_3rdparty PUBLIC Tracy::TracyClient)

# Tracy, Assimp, EASTL / jleSTL, glm, plog
target_include_directories(jleCore_3rdparty PUBLIC SYSTEM
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/git_submodules"
    "${CMAKE_CURRENT_SOURCE_DIR}/git_submodules/tracy/public/tracy"
    "${CMAKE_CURRENT_SOURCE_DIR}/git_submodules/jle_EASTL/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/git_submodules/jle_EASTL/jle_EABase/include/Common"
    "${CMAKE_CURRENT_SOURCE_DIR}/git_submodules/plog/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/git_submodules/glm"
    "${CMAKE_CURRENT_SOURCE_DIR}/lua"
    "${CMAKE_CURRENT_SOURCE_DIR}/lua/lua"
    "${CMAKE_CURRENT_BINARY_DIR}/git_submodules/assimp/include"
)

if (MSVC)
    add_subdirectory(detours)
    target_link_libraries(jleCore_3rdparty PRIVATE lib_detours)
endif ()