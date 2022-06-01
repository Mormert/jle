// Copyright (c) 2022. Johan Lind

#pragma once

#include "iTexture.h"
#include "Image.h"
#include "jleResourceHolder.h"

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
        std::shared_ptr<iTexture> LoadTextureFromPath(const std::string &path) {
            if (!jleResourceHolder<iTexture>::IsResourceLoaded(path)) {
                std::shared_ptr<iTexture> texture = CreateTextureFromImage(
                        *jleResourceHolder<Image>::LoadResourceFromFile(path));
                jleResourceHolder<iTexture>::StoreResource(texture, path);
                return texture;
            } else {
                return jleResourceHolder<iTexture>::GetResource(path);
            }
        }

    };
}