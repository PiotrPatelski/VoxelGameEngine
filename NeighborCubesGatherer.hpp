#pragma once
#include "ChunkCoord.hpp"
#include "RenderableChunk.hpp"
#include "VoxelTypes.hpp"
#include <glm/vec3.hpp>

namespace NeighborGatherer {

void gatherEastWestFace(const RenderableChunk* neighbor, int offsetX,
                        int chunkSize, int paddedX,
                        VoxelTypes::NeighborVoxelsMap& out);

void gatherNorthSouthFace(const RenderableChunk* neighbor, int offsetZ,
                          int chunkSize, int paddedZ,
                          VoxelTypes::NeighborVoxelsMap& out);

void gatherDiagonalXZ(const RenderableChunk* neighbor, int offsetX, int offsetZ,
                      int chunkSize, int paddedX, int paddedZ,
                      VoxelTypes::NeighborVoxelsMap& out);

VoxelTypes::NeighborVoxelsMap gatherNeighborsForCoord(
    const ChunkCoord& center,
    const std::function<const RenderableChunk*(const ChunkCoord&)>& getChunk,
    int chunkSize);

} // namespace NeighborGatherer