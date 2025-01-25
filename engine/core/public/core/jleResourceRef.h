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

#ifndef JLE_RESOURCEREF_H
#define JLE_RESOURCEREF_H

#include "jleBuildConfig.h"

#include "jlePath.h"
#include "serialization/jleSerialization.h"

#include <cereal/cereal.hpp>

class jleResourceInterface;
class jleResourceHolder;

template <typename T>
struct jleResourceRef {

    static_assert(std::is_base_of<jleResourceInterface, T>::value, "T must derive from jleResourceInterface");

    jleResourceRef() = default;

    explicit jleResourceRef(const jlePath &path, jleSerializationContext &ctx, bool loadLater = false) : _path{path}
    {
        if (!loadLater) {
            loadResource(ctx);
        }
    };

    template <class Archive>
    auto save_minimal(const Archive&) const -> std::enable_if_t<cereal::traits::is_text_archive<Archive>::value, std::string>;

    template <class Archive>
    auto save_minimal(const Archive&) const -> std::enable_if_t<!cereal::traits::is_text_archive<Archive>::value, jlePathHash>;

    template <class Archive>
    auto load_minimal(const Archive&, const std::string& value) -> std::enable_if_t<cereal::traits::is_text_archive<Archive>::value>;

    template <class Archive>
    auto load_minimal(Archive const &, const jlePathHash& value) -> std::enable_if_t<!cereal::traits::is_text_archive<Archive>::value>;

    void reloadWithNewPath(const jlePath &path, jleSerializationContext& ctx);

    // Load resource from file
    void loadResource(jleSerializationContext &ctx);

    // Save resource to file, if the resource implementation have a save function
    void saveResource(jleSerializationContext &ctx);

    template <class OTHER>
    jleResourceRef & operator=(const OTHER &other) { *_ptr = other; return *this; }

    std::shared_ptr<T> get() const { return std::static_pointer_cast<T>(_ptr); }
    [[nodiscard]] jlePath getPath() const { return _path; }

    explicit operator bool() const { return _ptr.get(); }
    explicit operator T &() { return *_ptr; }
    explicit operator const T &() const { return *_ptr; }
    T & operator*() const noexcept { return *_ptr; }
    T * operator->() const noexcept { return static_cast<T *>(_ptr.get()); }

private:
    jlePath _path;
    std::shared_ptr<jleResourceInterface> _ptr{};
};

#include "jleResourceRef.inl"

#endif // JLE_RESOURCEREF_H
