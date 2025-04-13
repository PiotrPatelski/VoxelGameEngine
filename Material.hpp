#pragma once

#include <string>
#include <unordered_map>
#include "Cube.hpp"

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

class Materials {
   public:
    Materials();
    ~Materials() = default;
    Materials(const Materials&) = delete;
    Materials(Materials&&) = delete;
    Materials& operator=(const Materials&) = delete;
    Materials& operator=(Materials&&) = delete;
    const Material& get(CubeType type) const { return materials.at(type); }
    const std::unordered_map<CubeType, Material>& get() const {
        return materials;
    }

   private:
    std::unordered_map<CubeType, Material> materials;
};