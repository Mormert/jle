// Copyright (c) 2023. Johan Lind

#include "jleResourceInterface.h"

bool
jleResourceInterface::hasFileExtension(const std::string &fileExtensionTest)
{
    const auto extensions = getFileAssociations();
    for (auto &extension : extensions) {
        if (extension == fileExtensionTest) {
            return true;
        }
    }
    return false;
}

std::string
jleResourceInterface::getPrimaryFileAssociation()
{
    return getFileAssociations()[0];
}

std::string
jleResourceInterface::getDotPrimaryFileExtension()
{
    return "." + getPrimaryFileAssociation();
}
