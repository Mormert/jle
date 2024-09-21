/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#ifndef JLE_COMPILE_HELPER
#define JLE_COMPILE_HELPER

#include "jleCommon.h"

// These macros are used to reduce compile times by having the serialisation function from cereal
// only be compiled once for the archives used in jle, in one .cpp file, and not spread out across multiple.

// Note: this is not needed, it is only used to reduce compile time.

#if JLE_BUILD_EDITOR

class jleJSONOutputArchive;
class jleJSONInputArchive;
class jleBinaryOutputArchive;
class jleBinaryInputArchive;
class jleImGuiArchive;
class jleImGuiArchiveInternal;

#define JLE_EXTERN_TEMPLATE_CEREAL_H(object)                                                                           \
    extern template void object::serialize<jleJSONOutputArchive>(jleJSONOutputArchive &);                              \
    extern template void object::serialize<jleJSONInputArchive>(jleJSONInputArchive &);                                \
    extern template void object::serialize<jleBinaryOutputArchive>(jleBinaryOutputArchive &);                          \
    extern template void object::serialize<jleBinaryInputArchive>(jleBinaryInputArchive &);                            \
    extern template void object::serialize<jleImGuiArchive>(jleImGuiArchive &);                                        \
    extern template void object::serialize<jleImGuiArchiveInternal>(jleImGuiArchiveInternal &);

#define JLE_EXTERN_TEMPLATE_CEREAL_CPP(object)                                                                         \
    template void object::serialize<jleJSONOutputArchive>(jleJSONOutputArchive &);                                     \
    template void object::serialize<jleJSONInputArchive>(jleJSONInputArchive &);                                       \
    template void object::serialize<jleBinaryOutputArchive>(jleBinaryOutputArchive &);                                 \
    template void object::serialize<jleBinaryInputArchive>(jleBinaryInputArchive &);                                   \
    template void object::serialize<jleImGuiArchive>(jleImGuiArchive &);                                               \
    template void object::serialize<jleImGuiArchiveInternal>(jleImGuiArchiveInternal &);

#else

class jleJSONOutputArchive;
class jleJSONInputArchive;
class jleBinaryOutputArchive;
class jleBinaryInputArchive;

#define JLE_EXTERN_TEMPLATE_CEREAL_H(object)                                                                           \
    extern template void object::serialize<jleJSONOutputArchive>(jleJSONOutputArchive &);                              \
    extern template void object::serialize<jleJSONInputArchive>(jleJSONInputArchive &);                                \
    extern template void object::serialize<jleBinaryOutputArchive>(jleBinaryOutputArchive &);                          \
    extern template void object::serialize<jleBinaryInputArchive>(jleBinaryInputArchive &);

#define JLE_EXTERN_TEMPLATE_CEREAL_CPP(object)                                                                         \
    template void object::serialize<jleJSONOutputArchive>(jleJSONOutputArchive &);                                     \
    template void object::serialize<jleJSONInputArchive>(jleJSONInputArchive &);                                       \
    template void object::serialize<jleBinaryOutputArchive>(jleBinaryOutputArchive &);                                 \
    template void object::serialize<jleBinaryInputArchive>(jleBinaryInputArchive &);

#endif

#endif // JLE_COMPILE_HELPER