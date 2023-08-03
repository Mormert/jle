// Copyright (c) 2023. Johan Lind

#ifndef JLE_RESOURCEREF_H
#define JLE_RESOURCEREF_H

#include <cereal/cereal.hpp>
#include "jlePath.h"

class jleResourceInterface;

template <typename T>
struct jleResourceRef {

    static_assert(std::is_base_of<jleResourceInterface, T>::value, "T must derive from jleResourceInterface");

    jleResourceRef() = default;

    explicit jleResourceRef(const jlePath &path, bool loadLater = false) : path{path}
    {
        if (!loadLater) {
            loadResource();
        }
    };

    template <class Archive>
    std::string save_minimal(Archive const &) const;

    template <class Archive>
    void load_minimal( Archive const &, std::string const & value );

    // Load resource from file
    void loadResource();

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
