// Copyright (c) 2022. Johan Lind

#include "jleImage.h"

#include "3rdparty/stb_image.h"

#include "plog/Log.h"

#include <algorithm>

bool jleImage::LoadFromFile(const std::string& path) {
    this->path = path;
    stbi_set_flip_vertically_on_load(false);
    image_data = stbi_load(path.c_str(), &w, &h, &nr_channels, 0);

    if (image_data) {
        return true;
    }
    return false;
}

jleImage::jleImage(const std::string& path) {
    this->path = path;
    LoadFromFile(path);
}

jleImage::jleImage(const jleImage& i) {
    std::copy(i.image_data,
              i.image_data + i.nr_channels * i.w * i.h,
              this->image_data);
    this->nr_channels = i.nr_channels;
    this->w = i.h;
    this->h = i.h;
}

jleImage& jleImage::operator=(const jleImage& i) {
    if (this == &i) {
        return *this;
    }
    if (image_data) {
        stbi_image_free(image_data);
    }

    std::copy(i.image_data,
              i.image_data + i.nr_channels * i.w * i.h,
              this->image_data);
    this->nr_channels = i.nr_channels;
    this->w = i.h;
    this->h = i.h;

    return *this;
}

jleImage::jleImage(jleImage&& i) noexcept {
    this->image_data = i.image_data;
    this->nr_channels = i.nr_channels;
    this->w = i.h;
    this->h = i.h;

    i.h = 0;
    i.w = 0;
    i.nr_channels = 1;
    i.image_data = nullptr;
}

jleImage& jleImage::operator=(jleImage&& i) noexcept {
    if (this != &i) {
        if (image_data) {
            stbi_image_free(image_data);
        }

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

jleImage::~jleImage() {
    if (image_data) {
        stbi_image_free(image_data);
    }

    PLOG_VERBOSE << "Destroyed image [" << w << ", " << h << "].";
}

unsigned int jleImage::GetImageHeight() const { return h; }

unsigned int jleImage::GetImageNrChannels() const { return nr_channels; }

unsigned char *jleImage::GetImageData() const { return image_data; }

unsigned int jleImage::GetImageWidth() const { return w; }

const std::string& jleImage::GetPath() const { return path; }

std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> jleImage::GetPixelAtLocation(
    uint32_t x, uint32_t y) const {
    if (x >= w) {
        LOGW << "x was outside bounds: " << x
             << ", can't be larger than or equal to " << w;
        return std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>{0, 0, 0, 0};
    }

    if (y >= h) {
        LOGW << "y was outside bounds: " << y
             << ", can't be larger than or equal to " << h;
        return std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>{0, 0, 0, 0};
    }

    int32_t bytesPerPixel = nr_channels;
    const uint8_t *pixelOffset = image_data + (x + w * y) * bytesPerPixel;
    const uint8_t r = pixelOffset[0];
    const uint8_t g = pixelOffset[1];
    const uint8_t b = pixelOffset[2];
    const uint8_t a = nr_channels >= 4 ? pixelOffset[3] : 0;

    LOGV << int(r) << " " << int(g) << " " << int(b) << " " << int(a);

    return std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>{r, g, b, a};
}
