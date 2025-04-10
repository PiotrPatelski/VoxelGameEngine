#pragma once

#include <vector>
#include "FastNoiseLite.h"

class GridGenerator {
   public:
    GridGenerator(int chunkSize, int worldXIndex, int worldZIndex);

    std::vector<std::vector<std::vector<bool>>> generateGrid();

   private:
    int chunkSize;
    int chunkWorldXPosition;
    int chunkWorldZPosition;
    FastNoiseLite noise;
};
