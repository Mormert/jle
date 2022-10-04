// Copyright (c) 2022. Johan Lind

#include "jleImage.h"

#include "stb_image.h"

#include "plog/Log.h"

#include <algorithm>

bool jleImage::loadFromFile(const std::string& path) {
    this->_path = path;
    stbi_set_flip_vertically_on_load(false);
    image_data = stbi_load(path.c_str(), &_width, &_height, &_nrChannels, 0);

    if (image_data) {
        return true;
    }
    return false;
}

jleImage::jleImage(const std::string& path) {
    this->_path = path;
    loadFromFile(path);
}

jleImage::jleImage(const jleImage& i) {
    std::copy(i.image_data,
              i.image_data + i._nrChannels * i._width * i._height,
              this->image_data);
    this->_nrChannels = i._nrChannels;
    this->_width = i._height;
    this->_height = i._height;
}

jleImage& jleImage::operator=(const jleImage& i) {
    if (this == &i) {
        return *this;
    }
    if (image_data) {
        stbi_image_free(image_data);
    }

    std::copy(i.image_data,
              i.image_data + i._nrChannels * i._width * i._height,
              this->image_data);
    this->_nrChannels = i._nrChannels;
    this->_width = i._height;
    this->_height = i._height;

    return *this;
}

jleImage::jleImage(jleImage&& i) noexcept {
    this->image_data = i.image_data;
    this->_nrChannels = i._nrChannels;
    this->_width = i._height;
    this->_height = i._height;

    i._height = 0;
    i._width = 0;
    i._nrChannels = 1;
    i.image_data = nullptr;
}

jleImage& jleImage::operator=(jleImage&& i) noexcept {
    if (this != &i) {
        if (image_data) {
            stbi_image_free(image_data);
        }

        this->image_data = i.image_data;
        this->_nrChannels = i._nrChannels;
        this->_width = i._height;
        this->_height = i._height;

        i._height = 0;
        i._width = 0;
        i._nrChannels = 1;
        i.image_data = nullptr;
    }
    return *this;
}

jleImage::~jleImage() {
    if (image_data) {
        stbi_image_free(image_data);
    }

    PLOG_VERBOSE << "Destroyed image [" << _width << ", " << _height << "].";
}

unsigned int jleImage::height() const { return _height; }

unsigned int jleImage::nrChannels() const { return _nrChannels; }

unsigned char *jleImage::data() const { return image_data; }

unsigned int jleImage::width() const { return _width; }

const std::string& jleImage::path() const { return _path; }

std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> jleImage::pixelAtLocation(
    uint32_t x, uint32_t y) const {
    if (x >= _width) {
        LOGW << "x was outside bounds: " << x
             << ", can't be larger than or equal to " << _width;
        return std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>{0, 0, 0, 0};
    }

    if (y >= _height) {
        LOGW << "y was outside bounds: " << y
             << ", can't be larger than or equal to " << _height;
        return std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>{0, 0, 0, 0};
    }

    int32_t bytesPerPixel = _nrChannels;
    const uint8_t *pixelOffset = image_data + (x + _width * y) * bytesPerPixel;
    const uint8_t r = pixelOffset[0];
    const uint8_t g = pixelOffset[1];
    const uint8_t b = pixelOffset[2];
    const uint8_t a = _nrChannels >= 4 ? pixelOffset[3] : 0;

    LOGV << int(r) << " " << int(g) << " " << int(b) << " " << int(a);

    return std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>{r, g, b, a};
}
