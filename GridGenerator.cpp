#include "GridGenerator.hpp"

namespace {
CubeType getCubeTypeBasedOnHeight(int y) {
    if (y < 11)
        return CubeType::SAND;
    else if (y < 14)
        return CubeType::DIRT;
    else
        return CubeType::GRASS;
}
} // namespace

GridGenerator::GridGenerator(int size, int worldXIndex, int worldZIndex)
    : chunkSize(size),
      chunkWorldXIndex(worldXIndex),
      chunkWorldZIndex(worldZIndex) {
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.02f);
}

VoxelTypes::VoxelGrid3D GridGenerator::generateGrid() {
    VoxelTypes::VoxelGrid3D grid(
        chunkSize,
        std::vector<std::vector<CubeType>>(
            chunkSize, std::vector<CubeType>(chunkSize, CubeType::NONE)));

    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            const auto worldCubeX = chunkWorldXIndex * chunkSize + x;
            const auto worldCubeZ = chunkWorldZIndex * chunkSize + z;
            const auto heightValue = noise.GetNoise(
                static_cast<float>(worldCubeX), static_cast<float>(worldCubeZ));
            const auto height =
                static_cast<int>((heightValue + 1.1f) * 0.7f * chunkSize / 2) -
                3;
            for (int y = 0; y < chunkSize; y++) {
                if (y <= height) grid[x][z][y] = getCubeTypeBasedOnHeight(y);
            }
        }
    }
    return grid;
}