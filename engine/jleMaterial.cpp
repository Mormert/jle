// Copyright (c) 2023. Johan Lind
#include "jleMaterial.h"

JLE_EXTERN_TEMPLATE_CEREAL_CPP(jleMaterial)


std::vector<std::string>
jleMaterial::getFileAssociationList()
{
    return {"mat"};
}

void
jleMaterial::useMaterial()
{
}
