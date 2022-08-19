// Copyright (c) 2022. Johan Lind

#pragma once

#include "iTexture.h"
#include "Image.h"
#include "jleResourceHolder.h"
#include "jlePath.h"

#include <memory>

namespace jle {
    class iTextureCreator {
    public:
        virtual ~iTextureCreator() {}

        // Returns a ptr to a texture based on provided image, with no consideration to
        // storing the resource for later use
        virtual std::unique_ptr<iTexture> CreateTextureFromImage(const Image &image) = 0;

        // Utilizes the resource holder such that only one copy of the resource will exists,
        // and if it does not exist, the program will construct it and store it for later use
        std::shared_ptr<iTexture> LoadTextureFromPath(const jleRelativePath &path) {
            if (!jleResourceHolder::IsResourceLoaded(path)) {

                std::shared_ptr<iTexture> texture = CreateTextureFromImage(
                        *jleResourceHolder::LoadResourceFromFile<Image>(path));

                jleResourceHolder::StoreResource<iTexture>(texture, path);

                return texture;
            } else {
                return jleResourceHolder::GetResource<iTexture>(path);
            }
        }

    };
}