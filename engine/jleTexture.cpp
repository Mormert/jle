// Copyright (c) 2023. Johan Lind

#include "jleTexture.h"

#include "stb_image.h"

#include "jleIncludeGL.h"

#include "jleCore.h"
#include "jleImage.h"
#include "jleResource.h"
#include "jleStaticOpenGLState.h"
#include "plog/Log.h"
#include <cereal/archives/json.hpp>

#include <fstream>
#include <iostream>
#include <memory>

jleTexture::~jleTexture()
{
    if (_id != UINT_MAX) {
        glDeleteTextures(1, &_id);
    }
    LOGV << "Destroyed texture with id " << _id;
}

bool
jleTexture::loadFromFile(const std::string &path)
{

    try {
        std::ifstream i(path);
        cereal::JSONInputArchive iarchive{i};
        iarchive(*this);
    } catch (std::exception &e) {
        LOGE << "Failed to load texture, using image directly from path: " << path << " instead.";
        imagePath = jleRelativePath{path};
    }

    gCore->resources().loadResourceFromFile<jleImage>(imagePath);
    jleImage image{imagePath.absolutePathStr()};

    _width = image.width();
    _height = image.height();
    _nrChannels = image.nrChannels();

    glGenTextures(1, &_id);

    glBindTexture(GL_TEXTURE_2D, _id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (image.data()) {
        GLenum format = GL_RGBA;
        if (image.nrChannels() == 1)
            format = GL_RED;
        else if (image.nrChannels() == 3)
            format = GL_RGB;
        else if (image.nrChannels() == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, _id);
        if (format == GL_RGB) {
            // Needed to load jpg images with different byte alignments
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        } else if (format == GL_RGBA) {
            // Byte alignment 4 is defaulted for RGBA images
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        }
        glTexImage2D(
            GL_TEXTURE_2D, 0, format, image.width(), image.height(), 0, format, GL_UNSIGNED_BYTE, image.data());
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        PLOG_VERBOSE << "Generated OpenGL texture " << _id << " (" << image.nrChannels() << " channels)";

        glBindTexture(GL_TEXTURE_2D, 0);
        jleStaticOpenGLState::globalActiveTexture = 0;

    } else {
        PLOG_ERROR << "Failed to generate OpenGL texture " << _id << " with path: " << image.path();

        return false;
    }

    return true;
}

bool
jleTexture::isActive()
{
    return jleStaticOpenGLState::globalActiveTexture == _id;
}

void
jleTexture::setActive(int texture_slot)
{
    glActiveTexture(GL_TEXTURE0 + texture_slot);
    glBindTexture(GL_TEXTURE_2D, _id);
    jleStaticOpenGLState::globalActiveTexture = _id;
}

int32_t
jleTexture::width()
{
    return _width;
}

int32_t
jleTexture::height()
{
    return _height;
}

unsigned int
jleTexture::id()
{
    return _id;
}

void
jleTexture::saveToFile()
{
    std::ofstream textureSave{filepath};
    cereal::JSONOutputArchive outputArchive(textureSave);
    this->serialize(outputArchive);
}

/*
std::shared_ptr<jleTexture> jleTexture::fromPath(const jleRelativePath &path) {
    auto &resources = gCore->resources();
    if (!resources.isResourceLoaded(path)) {

        auto texture = std::make_shared<jleTexture>(
            *resources.loadResourceFromFile<jleImage>(path));

        resources.storeResource<jleTexture>(texture, path);

        return texture;
    }
    else {
        return resources.resource<jleTexture>(path);
    }
}*/
