// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleImage.h"
#include "jlePath.h"
#include "jleResourceHolder.h"
#include "jleTextureInterface.h"

#include <memory>

class jleTextureCreatorInterface {
public:
    virtual ~jleTextureCreatorInterface() {}

    // Returns a ptr to a texture based on provided image, with no consideration
    // to storing the resource for later use
    virtual std::unique_ptr<jleTextureInterface> CreateTextureFromImage(
        const jleImage& image) = 0;

    // Utilizes the resource holder such that only one copy of the resource will
    // exists, and if it does not exist, the program will construct it and store
    // it for later use
    std::shared_ptr<jleTextureInterface> LoadTextureFromPath(const jleRelativePath& path) {
        if (!jleResourceHolder::IsResourceLoaded(path)) {

            std::shared_ptr<jleTextureInterface> texture = CreateTextureFromImage(
                *jleResourceHolder::LoadResourceFromFile<jleImage>(path));

            jleResourceHolder::StoreResource<jleTextureInterface>(texture, path);

            return texture;
        }
        else {
            return jleResourceHolder::GetResource<jleTextureInterface>(path);
        }
    }
};