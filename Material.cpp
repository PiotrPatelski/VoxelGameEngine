#include "Material.hpp"
#include <string>

namespace {
constexpr unsigned PLACEHOLDER_TEXTURE_UNIT{99};
constexpr float OPAQUE{1.0f};
constexpr float SEMI_TRANSPARENT{0.5f};

Material createSandMaterial() {
    const int textureUnit{1};
    const float shininess{32.0f};
    const bool isMultiTexture{false};
    return {"textures/sand.jpg",
            "",
            "",
            textureUnit,
            PLACEHOLDER_TEXTURE_UNIT,
            PLACEHOLDER_TEXTURE_UNIT,
            shininess,
            OPAQUE,
            isMultiTexture};
}

Material createDirtMaterial() {
    const int textureUnit{2};
    const float shininess{16.0f};
    const bool isMultiTexture{false};
    return {"textures/dirt.jpg",
            "",
            "",
            textureUnit,
            PLACEHOLDER_TEXTURE_UNIT,
            PLACEHOLDER_TEXTURE_UNIT,
            shininess,
            OPAQUE,
            isMultiTexture};
}

Material createGrassMaterial() {
    const int textureUnit{3};
    const float shininess{8.0f};
    const bool isMultiTexture{false};
    return {"textures/grass.jpg",
            "",
            "",
            textureUnit,
            PLACEHOLDER_TEXTURE_UNIT,
            PLACEHOLDER_TEXTURE_UNIT,
            shininess,
            OPAQUE,
            isMultiTexture};
}

Material createWaterMaterial() {
    const int textureUnit{4};
    const float shininess{64.0f};
    const bool isMultiTexture{false};
    return {"textures/water.jpg",
            "",
            "",
            textureUnit,
            PLACEHOLDER_TEXTURE_UNIT,
            PLACEHOLDER_TEXTURE_UNIT,
            shininess,
            SEMI_TRANSPARENT,
            isMultiTexture};
}

Material createLogMaterial() {
    const int maintextureUnit{5};
    const int secondarytextureUnit{6};
    const float shininess{32.0f};
    const bool isMultiTexture{true};
    return {"textures/logBark.jpg",
            "textures/logInside.jpg",
            "",
            maintextureUnit,
            secondarytextureUnit,
            PLACEHOLDER_TEXTURE_UNIT,
            shininess,
            OPAQUE,
            isMultiTexture};
}

Material createLeavesMaterial() {
    const int textureUnit{7};
    const float shininess{8.0f};
    const bool isMultiTexture{false};
    return {"textures/leaves.jpg",
            "",
            "",
            textureUnit,
            PLACEHOLDER_TEXTURE_UNIT,
            PLACEHOLDER_TEXTURE_UNIT,
            shininess,
            OPAQUE,
            isMultiTexture};
}

Material createTorchMaterial() {
    const int textureUnit{8};
    const float shininess{32.0f};
    const bool isMultiTexture{false};
    return {"textures/lamp.png",
            "",
            "",
            textureUnit,
            PLACEHOLDER_TEXTURE_UNIT,
            PLACEHOLDER_TEXTURE_UNIT,
            shininess,
            OPAQUE,
            isMultiTexture};
}
} // namespace

Materials::Materials() {
    materials[CubeType::SAND] = createSandMaterial();
    materials[CubeType::DIRT] = createDirtMaterial();
    materials[CubeType::GRASS] = createGrassMaterial();
    materials[CubeType::WATER_SOURCE] = createWaterMaterial();
    materials[CubeType::WATER_FLOWING] = createWaterMaterial();
    materials[CubeType::LOG] = createLogMaterial();
    materials[CubeType::LEAVES] = createLeavesMaterial();
    materials[CubeType::TORCH] = createTorchMaterial();
}