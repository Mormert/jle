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

#include "jleResourceRef.h"

#include "jleResourceHolder.h"

template <typename T>
void
jleResourceRef<T>::loadResource(jleSerializationContext &ctx)
{
    _ptr = nullptr;
    if (!_path.isEmpty()) {
        _ptr = ctx.resources->loadResourceFromFileT<T>(_path, ctx);
    }
}

template <typename T>
void
jleResourceRef<T>::reloadWithNewPath(const jlePath &newPath, jleSerializationContext& ctx)
{
    _path = newPath;
    loadResource(ctx);
}

template <typename T>
void
jleResourceRef<T>::saveResource(jleSerializationContext &ctx)
{
    _ptr->saveToFile(ctx);
}

template<typename T>
template<class Archive>
auto jleResourceRef<T>::save_minimal(const Archive &) const -> std::enable_if_t<cereal::traits::is_text_archive<Archive>::value, std::string> {
    // Save the path in a human-readable format for JSON formats
    return _path.getVirtualPath().str();
}

template<typename T>
template<class Archive>
auto jleResourceRef<T>::save_minimal(const Archive &) const -> std::enable_if_t<!cereal::traits::is_text_archive<Archive>::value, jlePathHash> {
    // Save the path as a non human-readable integer hash for binary formats
    return _path.getHash();
}

template<typename T>
template<class Archive>
auto jleResourceRef<T>::load_minimal(const Archive &ar, const std::string &value) -> std::enable_if_t<cereal::traits::is_text_archive<Archive>::value> {
    // Loads the path from a virtual path
    _path = jlePath{jleVirtualPath{value}};
    _ptr = nullptr;
    if (!_path.isEmpty()) {
        jleSerializationContext &ctx = const_cast<jleSerializationContext&>(ar.ctx);
        jleAssert(ctx.resources);
        _ptr = ctx.resources->loadResourceFromFileT<T>(_path, ctx);
    }
}

template<typename T>
template<class Archive>
auto jleResourceRef<T>::load_minimal(Archive const &ar, const jlePathHash &value) -> std::enable_if_t<!cereal::traits::is_text_archive<Archive>::value> {
    // Loads the path from a path hash
    _path = jlePath{value};
    _ptr = nullptr;
    if (!_path.isEmpty()) {
        jleSerializationContext &ctx = const_cast<jleSerializationContext&>(ar.ctx);
        jleAssert(ctx.resources);
        _ptr = ctx.resources->loadResourceFromFileT<T>(_path, ctx);
    }
}
