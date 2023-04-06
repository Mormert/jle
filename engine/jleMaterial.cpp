// Copyright (c) 2023. Johan Lind

#include "jleMaterial.h"
#include "jleCore.h"
#include "jleResource.h"
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>
#include <fstream>
#include <plog/Log.h>

jleMaterial::~jleMaterial() = default;

jleLoadFromFileSuccessCode
jleMaterial::loadFromFile(const jlePath &path)
{

    try {
        std::ifstream i(path.getRealPath());
        cereal::JSONInputArchive iarchive{i};
        iarchive(*this);
    } catch (std::exception &e) {
        LOGE << "Failed loading material: " << e.what();
    }

    std::ifstream i(path.getRealPath());
    if (i.good()) {


    } else {
        LOG_ERROR << "Could not load scene with path: ";
        return jleLoadFromFileSuccessCode::FAIL;
    }

    if (!_albedoPath.empty()) {
        albedoTexture = gCore->resources().loadResourceFromFile<jleTexture>(jlePath{_albedoPath});
    }

    if (!_normalPath.empty()) {
        normalTexture = gCore->resources().loadResourceFromFile<jleTexture>(jlePath{_normalPath});
    }

    if (!_metallicPath.empty()) {
        metallicTexture = gCore->resources().loadResourceFromFile<jleTexture>(jlePath{_metallicPath});
    }

    if (!_roughnessPath.empty()) {
        roughnessTexture = gCore->resources().loadResourceFromFile<jleTexture>(jlePath{_roughnessPath});
    }

    return jleLoadFromFileSuccessCode::SUCCESS;
}

void
jleMaterial::saveToFile()
{
    std::ofstream materialSave{filepath};
    cereal::JSONOutputArchive outputArchive(materialSave);
    this->serialize(outputArchive);
}
