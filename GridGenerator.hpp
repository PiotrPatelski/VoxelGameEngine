#pragma once

#include <vector>
#include "FastNoiseLite.h"
#include "Cube.hpp"

class GridGenerator {
   public:
    GridGenerator(int chunkSize, int worldXIndex, int worldZIndex);
    GridGenerator(const GridGenerator&) = delete;
    GridGenerator(GridGenerator&&) = delete;
    GridGenerator& operator=(const GridGenerator&) = delete;
    GridGenerator& operator=(GridGenerator&&) = delete;
    using VoxelGrid = std::vector<std::vector<std::vector<CubeType>>>;
    VoxelGrid generateGrid();

   private:
    int chunkSize{};
    int chunkWorldXPosition{};
    int chunkWorldZPosition{};
    FastNoiseLite noise{};
};
