// Copyright (c) 2023. Johan Lind

#include "jleMaterial.h"
#include "jleCore.h"
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>
#include <fstream>
#include <plog/Log.h>

jleMaterial::~jleMaterial() = default;

bool
jleMaterial::loadFromFile(const std::string &path)
{

    try {
        std::ifstream i(path);
        cereal::JSONInputArchive iarchive{i};
        iarchive(*this);
    } catch (std::exception &e) {
        LOGE << "Failed loading material: " << e.what();
    }

    std::ifstream i(path);
    if (i.good()) {


    } else {
        LOG_ERROR << "Could not load scene with path: ";
        return false;
    }

    if (!_albedoPath.empty()) {
        albedoTexture = gCore->resources().loadResourceFromFile<jleTexture>(jleRelativePath{_albedoPath});
    }

    if (!_normalPath.empty()) {
        normalTexture = gCore->resources().loadResourceFromFile<jleTexture>(jleRelativePath{_normalPath});
    }

    if (!_metallicPath.empty()) {
        metallicTexture = gCore->resources().loadResourceFromFile<jleTexture>(jleRelativePath{_metallicPath});
    }

    if (!_roughnessPath.empty()) {
        roughnessTexture = gCore->resources().loadResourceFromFile<jleTexture>(jleRelativePath{_roughnessPath});
    }

    return true;
}

void
jleMaterial::saveToFile()
{
    std::ofstream materialSave{filepath};
    cereal::JSONOutputArchive outputArchive(materialSave);
    this->serialize(outputArchive);
}
