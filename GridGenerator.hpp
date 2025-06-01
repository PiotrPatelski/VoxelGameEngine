#pragma once

#include <vector>
#include "FastNoiseLite.h"
#include "Cube.hpp"
#include "VoxelTypes.hpp"

class GridGenerator {
   public:
    GridGenerator(int chunkSize, int worldXIndex, int worldZIndex);
    GridGenerator(const GridGenerator&) = delete;
    GridGenerator(GridGenerator&&) = delete;
    GridGenerator& operator=(const GridGenerator&) = delete;
    GridGenerator& operator=(GridGenerator&&) = delete;
    VoxelTypes::VoxelGrid3D generateGrid();

   private:
    int chunkSize{};
    int chunkWorldXIndex{};
    int chunkWorldZIndex{};
    FastNoiseLite noise{};
};
