// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFileLoadInterface.h"

#include "jleTexture.h"
#include "json.hpp"

class jleMaterial : public jleFileLoadInterface
{
public:
    ~jleMaterial() override;

    bool loadFromFile(const std::string &path) override;

    std::shared_ptr<jleTexture> albedoTexture{};
    std::shared_ptr<jleTexture> normalTexture{};
    std::shared_ptr<jleTexture> metallicTexture{};
    std::shared_ptr<jleTexture> roughnessTexture{};

    friend void to_json(nlohmann::json &j, const jleMaterial &m);
    friend void from_json(const nlohmann::json &j, jleMaterial &m);

protected:
    std::string _albedoPath;
    std::string _normalPath;
    std::string _metallicPath;
    std::string _roughnessPath;

};
