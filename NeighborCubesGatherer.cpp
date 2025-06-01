#include "NeighborCubesGatherer.hpp"

NeighborGatherer::NeighborGatherer(int size) : chunkSize{size} {}

void NeighborGatherer::gatherEastWestFace(const RenderableChunk* neighbor,
                                          VoxelTypes::NeighborVoxelsMap& out) {
    int localX = (offsetX < 0 ? chunkSize - 1 : 0);
    for (int y = 0; y < chunkSize; ++y) {
        for (int z = 0; z < chunkSize; ++z) {
            if (neighbor->isValidCubeAt({localX, y, z})) {
                const glm::ivec3 paddedPos{paddedX, y + 1, z + 1};
                out[paddedPos] = neighbor->getCubeType({localX, y, z});
            }
        }
    }
}

void NeighborGatherer::gatherNorthSouthFace(
    const RenderableChunk* neighbor, VoxelTypes::NeighborVoxelsMap& out) {
    int localZ = (offsetZ < 0 ? chunkSize - 1 : 0);
    for (int x = 0; x < chunkSize; ++x) {
        for (int y = 0; y < chunkSize; ++y) {
            if (neighbor->isValidCubeAt({x, y, localZ})) {
                const glm::ivec3 paddedPos{x + 1, y + 1, paddedZ};
                out[paddedPos] = neighbor->getCubeType({x, y, localZ});
            }
        }
    }
}

void NeighborGatherer::gatherDiagonalXZ(const RenderableChunk* neighbor,
                                        VoxelTypes::NeighborVoxelsMap& out) {
    int localX = (offsetX < 0 ? chunkSize - 1 : 0);
    int localZ = (offsetZ < 0 ? chunkSize - 1 : 0);
    for (int y = 0; y < chunkSize; ++y) {
        if (neighbor->isValidCubeAt({localX, y, localZ})) {
            const glm::ivec3 paddedPos{paddedX, y + 1, paddedZ};
            out[paddedPos] = neighbor->getCubeType({localX, y, localZ});
        }
    }
}

void NeighborGatherer::gatherNeighborFaces(const RenderableChunk* neighbor,
                                           VoxelTypes::NeighborVoxelsMap& out) {
    if (not neighbor) {
        return;
    } else if (offsetX != 0 && offsetZ == 0) {
        paddedX = (offsetX < 0 ? 0 : chunkSize + 1);
        gatherEastWestFace(neighbor, out);
    } else if (offsetX == 0 && offsetZ != 0) {
        paddedZ = (offsetZ < 0 ? 0 : chunkSize + 1);
        gatherNorthSouthFace(neighbor, out);
    } else {
        paddedX = (offsetX < 0 ? 0 : chunkSize + 1);
        paddedZ = (offsetZ < 0 ? 0 : chunkSize + 1);
        gatherDiagonalXZ(neighbor, out);
    }
}

VoxelTypes::NeighborVoxelsMap NeighborGatherer::gatherNeighborsForCoord(
    const ChunkCoord& center,
    const std::function<const RenderableChunk*(const ChunkCoord&)>& getChunk) {
    VoxelTypes::NeighborVoxelsMap result;

    for (offsetX = -1; offsetX <= 1; ++offsetX) {
        for (offsetZ = -1; offsetZ <= 1; ++offsetZ) {
            if (offsetX == 0 && offsetZ == 0) {
                continue;
            }

            ChunkCoord neighborCoord{center.x + offsetX, center.z + offsetZ};
            const RenderableChunk* neighbor = getChunk(neighborCoord);
            gatherNeighborFaces(neighbor, result);
        }
    }
    return result;
}
