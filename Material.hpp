#pragma once

#include <string>

struct Material {
    std::string diffuseTexturePath{};
    std::string emissionTexturePath{};
    int diffuseUnit{};
    int emissionUnit{};
    float shininess{};
    float alpha{};
};