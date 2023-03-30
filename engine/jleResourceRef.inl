// Copyright (c) 2023. Johan Lind

#include "jleResourceRef.h"
#include "jleCore.h"

template <typename T>
template <class Archive>
void
jleResourceRef<T>::save(Archive &ar) const
{
    ar(CEREAL_NVP(path));
}

template <typename T>
template <class Archive>
void
jleResourceRef<T>::load(Archive &ar)
{
    ar(CEREAL_NVP(path));

    ptr = nullptr;
    if (!path.isEmpty()) {
        ptr = gCore->resources().loadResourceFromFile<T>(path);
    }
}

template <typename T>
void
jleResourceRef<T>::loadResource()
{
    ptr = nullptr;
    if (!path.isEmpty()) {
        ptr = gCore->resources().loadResourceFromFile<T>(path);
    }
}

template <typename T>
void
jleResourceRef<T>::saveResource()
{
    ptr->saveToFile();
}