#pragma once
#include "ChunkCoord.hpp"
#include "RenderableChunk.hpp"
#include "VoxelTypes.hpp"
#include <glm/vec3.hpp>

class NeighborGatherer {
   public:
    NeighborGatherer(int size);
    ~NeighborGatherer() = default;
    NeighborGatherer(const NeighborGatherer&) = delete;
    NeighborGatherer& operator=(const NeighborGatherer&) = delete;
    NeighborGatherer(NeighborGatherer&&) = delete;
    NeighborGatherer& operator=(NeighborGatherer&&) = delete;

    VoxelTypes::NeighborVoxelsMap gatherNeighborsForCoord(
        const ChunkCoord& center,
        const std::function<const RenderableChunk*(const ChunkCoord&)>&
            getChunk);

   private:
    void gatherEastWestFace(const RenderableChunk* neighbor,
                            VoxelTypes::NeighborVoxelsMap& out);
    void gatherNorthSouthFace(const RenderableChunk* neighbor,
                              VoxelTypes::NeighborVoxelsMap& out);
    void gatherDiagonalXZ(const RenderableChunk* neighbor,
                          VoxelTypes::NeighborVoxelsMap& out);
    void gatherNeighborFaces(const RenderableChunk* neighbor,
                             VoxelTypes::NeighborVoxelsMap& out);
    int chunkSize{0};
    int offsetX{0};
    int offsetZ{0};
    int paddedX{0};
    int paddedZ{0};
};