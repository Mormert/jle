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

#include <cereal/cereal.hpp>
#include "jlePath.h"

class jleResourceInterface;
class jleResources;

template <typename T>
struct jleResourceRef {

    static_assert(std::is_base_of<jleResourceInterface, T>::value, "T must derive from jleResourceInterface");

    jleResourceRef() = default;

    explicit jleResourceRef(const jlePath &path, jleResources& resources ,bool loadLater = false) : path{path}
    {
        if (!loadLater) {
            loadResource(resources);
        }
    };

    template <class Archive>
    std::string save_minimal(Archive const &) const;

    template <class Archive>
    void load_minimal( Archive const &, std::string const & value );

    void reloadWithNewPath(const jlePath& path, jleResources& resources);

    // Load resource from file
    void loadResource(jleResources& resources);

    // Save resource to file, if the resource implementation have a save function
    void saveResource();

    template <class OTHER>
    jleResourceRef &
    operator=(const OTHER &other)
    {
        *ptr = other;
        return *this;
    }

    explicit operator bool() const { return ptr.get(); }

    std::shared_ptr<T>
    get()
    {
        return std::static_pointer_cast<T>(ptr);
    }

    explicit
    operator T &()
    {
        return *ptr;
    }

    explicit operator const T &() const { return *ptr; }

    T &
    operator*() const noexcept
    {
        return *ptr;
    }

    T *
    operator->() const noexcept
    {
        T *p = static_cast<T *>(ptr.get());
        return p;
    }

    jlePath path{};

private:
    std::shared_ptr<jleResourceInterface> ptr{};
};

#include "jleResourceRef.inl"

#endif // JLE_RESOURCEREF_H
