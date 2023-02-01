// Copyright (c) 2023. Johan Lind

#include "jleResourceRef.h"

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