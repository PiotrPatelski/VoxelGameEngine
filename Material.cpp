#include "Material.hpp"
#include <string>

namespace {
constexpr unsigned placeHolderTextureUnit{99};
constexpr float opaque{1.0f};
constexpr float semiTransparent{0.5f};

Material createSandMaterial() {
    const int textureUnit{1};
    const float shininess{32.0f};
    const bool isMultiTexture{false};
    return {"textures/sand.jpg",
            "",
            "",
            textureUnit,
            placeHolderTextureUnit,
            placeHolderTextureUnit,
            shininess,
            opaque,
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
            placeHolderTextureUnit,
            placeHolderTextureUnit,
            shininess,
            opaque,
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
            placeHolderTextureUnit,
            placeHolderTextureUnit,
            shininess,
            opaque,
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
            placeHolderTextureUnit,
            placeHolderTextureUnit,
            shininess,
            semiTransparent,
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
            placeHolderTextureUnit,
            shininess,
            opaque,
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
            placeHolderTextureUnit,
            placeHolderTextureUnit,
            shininess,
            opaque,
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
            placeHolderTextureUnit,
            placeHolderTextureUnit,
            shininess,
            opaque,
            isMultiTexture};
}
} // namespace

Materials::Materials() {
    materials[CubeType::SAND] = createSandMaterial();
    materials[CubeType::DIRT] = createDirtMaterial();
    materials[CubeType::GRASS] = createGrassMaterial();
    materials[CubeType::WATER] = createWaterMaterial();
    materials[CubeType::LOG] = createLogMaterial();
    materials[CubeType::LEAVES] = createLeavesMaterial();
    materials[CubeType::TORCH] = createTorchMaterial();
}