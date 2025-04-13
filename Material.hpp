#pragma once

#include <string>

struct Material {
    std::string mainDiffuseTexturePath{};
    std::string secondaryDiffuseTexturePath{};
    std::string emissionTexturePath{};
    int mainDiffuseUnit{};
    int secondaryDiffuseUnit{};
    int emissionUnit{};
    float shininess{};
    float alpha{};
    bool useSecondaryTexture{};
};