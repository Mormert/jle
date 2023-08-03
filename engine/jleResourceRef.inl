// Copyright (c) 2023. Johan Lind

#include "jleResourceRef.h"

#include "jleGameEngine.h"
#include "jleResource.h"

template <typename T>
void
jleResourceRef<T>::loadResource()
{
    ptr = nullptr;
    if (!path.isEmpty()) {
        ptr = gEngine->resources().loadResourceFromFile<T>(path);
    }
}

template <typename T>
void
jleResourceRef<T>::saveResource()
{
    ptr->saveToFile();
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
jleResourceRef<T>::load_minimal(const Archive &, const std::string &value)
{
    path = jlePath{value};
    ptr = nullptr;
    if (!path.isEmpty()) {
        ptr = gEngine->resources().loadResourceFromFile<T>(path);
    }
}
