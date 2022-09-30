// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFileLoadInterface.h"

#include <string>

class jleImage : jleFileLoadInterface {
public:
    jleImage() = default;

    explicit jleImage(const std::string& path);

    bool LoadFromFile(const std::string& path) override;

    jleImage(const jleImage& i);

    jleImage& operator=(const jleImage& i);

    jleImage(jleImage&& i) noexcept;

    jleImage& operator=(jleImage&& i) noexcept;

    ~jleImage() override;

    [[nodiscard]] unsigned int GetImageWidth() const;

    [[nodiscard]] unsigned int GetImageHeight() const;

    [[nodiscard]] unsigned int GetImageNrChannels() const;

    [[nodiscard]] unsigned char *GetImageData() const;

    [[nodiscard]] std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>
    GetPixelAtLocation(uint32_t x, uint32_t y) const;

    [[nodiscard]] const std::string& GetPath() const;

private:
    unsigned char *image_data{nullptr};
    int w = 0, h = 0, nr_channels = 0;

    std::string path;
};
