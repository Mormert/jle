// Copyright (c) 2022. Johan Lind

#pragma once

#include "FileLoadInterface.h"

#include <string>

namespace jle {
    class Image : FileLoadInterface {
    public:
        Image() = default;

        explicit Image(const std::string &path);

        bool LoadFromFile(const std::string &path) override;

        Image(const Image &i);

        Image &operator=(const Image &i);

        Image(Image &&i) noexcept;

        Image &operator=(Image &&i) noexcept;

        ~Image() override;

        [[nodiscard]] unsigned int GetImageWidth() const;

        [[nodiscard]] unsigned int GetImageHeight() const;

        [[nodiscard]] unsigned int GetImageNrChannels() const;

        [[nodiscard]] unsigned char *GetImageData() const;

        [[nodiscard]] const std::string &GetPath() const;

    private:
        unsigned char *image_data{nullptr};
        int w = 0, h = 0, nr_channels = 0;

        std::string path;
    };
}