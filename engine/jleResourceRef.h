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

    explicit jleResourceRef(const jlePath &path) : path{path} {};

    // Serialization save
    template <class Archive>
    void save(Archive &ar) const;

    // Serialization load
    template <class Archive>
    void load(Archive &ar);

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
        return std::static_pointer_cast<T>(ptr);;
    }

    explicit
    operator T &()
    {
        return *ptr;
    }

    explicit operator const T &() const { return *ptr; }

    jlePath path{};

private:
    std::shared_ptr<jleResourceInterface> ptr{};
};

#include "jleResourceRef.inl"

#endif // JLE_RESOURCEREF_H
