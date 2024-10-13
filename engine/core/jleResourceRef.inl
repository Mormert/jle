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

#include "jleGameEngine.h"
#include "jleResource.h"

template <typename T>
void
jleResourceRef<T>::loadResource(jleSerializationContext &ctx)
{
    ptr = nullptr;
    if (!path.isEmpty()) {
        ptr = ctx.resources->loadResourceFromFileT<T>(path, ctx);
    }
}

template <typename T>
void
jleResourceRef<T>::reloadWithNewPath(const jlePath &newPath, jleSerializationContext& ctx)
{
    path = newPath;
    loadResource(ctx);
}

template <typename T>
void
jleResourceRef<T>::saveResource(jleSerializationContext &ctx)
{
    ptr->saveToFile(ctx);
}

template <typename T>
template <class Archive>
std::string
jleResourceRef<T>::save_minimal(const Archive &) const
{
    return path.getVirtualPath();
}

template <typename T>
template <class Archive>
void
jleResourceRef<T>::load_minimal(const Archive &ar, const std::string &value)
{
    path = jlePath{value};
    ptr = nullptr;
    if (!path.isEmpty()) {
        jleSerializationContext &ctx = const_cast<jleSerializationContext&>(ar.ctx);
        jleAssert(ctx.resources);
        ptr = ctx.resources->loadResourceFromFileT<T>(path, ctx);
    }
}
