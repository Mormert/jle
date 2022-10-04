// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFileLoadInterface.h"

#include <string>

class jleImage : jleFileLoadInterface {
public:
    jleImage() = default;

    explicit jleImage(const std::string& path);

    bool loadFromFile(const std::string& path) override;

    jleImage(const jleImage& i);

    jleImage& operator=(const jleImage& i);

    jleImage(jleImage&& i) noexcept;

    jleImage& operator=(jleImage&& i) noexcept;

    ~jleImage() override;

    [[nodiscard]] unsigned int imageWidth() const;

    [[nodiscard]] unsigned int imageHeight() const;

    [[nodiscard]] unsigned int imageNrChannels() const;

    [[nodiscard]] unsigned char *imageData() const;

    [[nodiscard]] std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>
    pixelAtLocation(uint32_t x, uint32_t y) const;

    [[nodiscard]] const std::string& path() const;

private:
    unsigned char *image_data{nullptr};
    int w = 0, h = 0, nr_channels = 0;

    std::string _path;
};
