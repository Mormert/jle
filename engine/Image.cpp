// Copyright (c) 2022. Johan Lind

#include "Image.h"

#include "3rdparty/stb_image.h"

#include "plog/Log.h"

#include <algorithm>

namespace jle {

    bool Image::LoadFromFile(const std::string &path) {
        this->path = path;
        stbi_set_flip_vertically_on_load(false);
        image_data = stbi_load(path.c_str(), &w, &h, &nr_channels, 0);

        if (image_data) {
            return true;
        }
        return false;
    }

    Image::Image(const std::string &path) {
        this->path = path;
        LoadFromFile(path);
    }

    Image::Image(const Image &i) {
        std::copy(i.image_data, i.image_data + i.nr_channels * i.w * i.h, this->image_data);
        this->nr_channels = i.nr_channels;
        this->w = i.h;
        this->h = i.h;
    }

    Image &Image::operator=(const Image &i) {
        if (this == &i) { return *this; }
        if (image_data) { stbi_image_free(image_data); }

        std::copy(i.image_data, i.image_data + i.nr_channels * i.w * i.h, this->image_data);
        this->nr_channels = i.nr_channels;
        this->w = i.h;
        this->h = i.h;

        return *this;
    }

    Image::Image(Image &&i) noexcept {
        this->image_data = i.image_data;
        this->nr_channels = i.nr_channels;
        this->w = i.h;
        this->h = i.h;

        i.h = 0;
        i.w = 0;
        i.nr_channels = 1;
        i.image_data = nullptr;
    }

    Image &Image::operator=(Image &&i) noexcept {
        if (this != &i) {
            if (image_data) { stbi_image_free(image_data); }

            this->image_data = i.image_data;
            this->nr_channels = i.nr_channels;
            this->w = i.h;
            this->h = i.h;

            i.h = 0;
            i.w = 0;
            i.nr_channels = 1;
            i.image_data = nullptr;
        }
        return *this;
    }

    Image::~Image() {
        if (image_data) { stbi_image_free(image_data); }

        PLOG_VERBOSE << "Destroyed image [" << w << ", " << h << "].";
    }

    unsigned int Image::GetImageHeight() const {
        return h;
    }

    unsigned int Image::GetImageNrChannels() const {
        return nr_channels;
    }

    unsigned char *Image::GetImageData() const {
        return image_data;
    }

    unsigned int Image::GetImageWidth() const {
        return w;
    }

    const std::string &Image::GetPath() const {
        return path;
    }

    std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> Image::GetPixelAtLocation(uint32_t x, uint32_t y) const {
        if(x >= w){
            LOGW << "x was outside bounds: " << x << ", can't be larger than or equal to " << w;
            return std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>{0,0,0,0};
        }

        if(y >= h){
            LOGW << "y was outside bounds: " << y << ", can't be larger than or equal to " << h;
            return std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>{0,0,0,0};
        }

        int32_t bytesPerPixel = nr_channels;
        const uint8_t* pixelOffset = image_data + (x + w * y) * bytesPerPixel;
        const uint8_t r = pixelOffset[0];
        const uint8_t g = pixelOffset[1];
        const uint8_t b = pixelOffset[2];
        const uint8_t a = nr_channels >= 4 ? pixelOffset[3] : 0;

        LOGV << int(r) << " " << int(g) << " " << int(b) << " " << int(a);

        return std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>{r,g,b,a};
    }
}