// Copyright (c) 2023. Johan Lind

#ifndef JLE_RESOURCEREF_H
#define JLE_RESOURCEREF_H

#include <cereal/cereal.hpp>

#include "jleCore.h"
#include "jlePath.h"
#include "jleResource.h"

template <typename T>
struct jleResourceRef {

    static_assert(std::is_base_of<jleFileLoadInterface, T>::value, "T must derive from FileLoadInterface");

    jleResourceRef() = default;

    explicit jleResourceRef(const jleRelativePath &path) : path{path} {};

    template <class Archive>
    void save(Archive &ar) const;

    template <class Archive>
    void load(Archive &ar);

    template <class OTHER>
    jleResourceRef &
    operator=(const OTHER &other)
    {
        *ptr = other;
        return *this;
    }

    explicit operator bool() const { return ptr.get(); }

    std::shared_ptr<T> &
    get()
    {
        return ptr;
    }

    explicit
    operator T &()
    {
        return *ptr;
    }

    explicit operator const T &() const { return *ptr; }

    jleRelativePath path{};

private:
    std::shared_ptr<T> ptr{};
};

#include "jleResourceRef.inl"

#endif // JLE_RESOURCEREF_H
