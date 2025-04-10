#include "GridGenerator.hpp"

GridGenerator::GridGenerator(int size, int worldXIndex, int worldZIndex)
    : chunkSize(size),
      chunkWorldXPosition(worldXIndex),
      chunkWorldZPosition(worldZIndex) {
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.02f);
}

GridGenerator::Grid GridGenerator::generateGrid() {
    Grid grid(chunkSize, std::vector<std::vector<bool>>(
                             chunkSize, std::vector<bool>(chunkSize, false)));

    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            int worldCubeX = chunkWorldXPosition * chunkSize + x;
            int worldCubeZ = chunkWorldZPosition * chunkSize + z;
            float heightValue = noise.GetNoise(static_cast<float>(worldCubeX),
                                               static_cast<float>(worldCubeZ));
            int height =
                static_cast<int>((heightValue + 1.1f) * 0.7f * chunkSize / 2) -
                3;
            for (int y = 0; y < chunkSize; y++) {
                if (y <= height) grid[x][z][y] = true;
            }
        }
    }
    return grid;
}