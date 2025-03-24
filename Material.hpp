#pragma once

#include <string>

struct Material {
    std::string diffuseTexturePath;
    std::string specularTexturePath;
    std::string emissionTexturePath;
    int diffuseUnit;
    int specularUnit;
    int emissionUnit;
    float shininess;
};