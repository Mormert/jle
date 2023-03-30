// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleResourceInterface.h"

#include <string>

class jleImage : public jleResourceInterface
{
public:
    jleImage() = default;

    explicit jleImage(const std::string &path);

    bool loadFromFile(const std::string &path) override;

    jleImage(const jleImage &i);

    jleImage &operator=(const jleImage &i);

    jleImage(jleImage &&i) noexcept;

    jleImage &operator=(jleImage &&i) noexcept;

    ~jleImage() override;

    [[nodiscard]] unsigned int width() const;

    [[nodiscard]] unsigned int height() const;

    [[nodiscard]] unsigned int nrChannels() const;

    [[nodiscard]] unsigned char *data() const;

    [[nodiscard]] std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> pixelAtLocation(uint32_t x, uint32_t y) const;

    [[nodiscard]] const std::string &path() const;

    static void setFlipImage(bool flip);

private:
    unsigned char *image_data{nullptr};
    int _width = 0, _height = 0, _nrChannels = 0;

    std::string _path;
};
