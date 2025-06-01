#include "NeighborCubesGatherer.hpp"
#include <glm/gtc/type_ptr.hpp> // if needed

namespace {
void gatherEastWestFace(const RenderableChunk* neighbor, int offsetX,
                        int chunkSize, int paddedX,
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

void gatherNorthSouthFace(const RenderableChunk* neighbor, int offsetZ,
                          int chunkSize, int paddedZ,
                          VoxelTypes::NeighborVoxelsMap& out) {
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

void gatherDiagonalXZ(const RenderableChunk* neighbor, int offsetX, int offsetZ,
                      int chunkSize, int paddedX, int paddedZ,
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

void gatherNeighborFaces(const RenderableChunk* neighbor, int offsetX,
                         int offsetZ, int chunkSize,
                         VoxelTypes::NeighborVoxelsMap& out) {
    if (not neighbor) {
        return;
    } else if (offsetX != 0 && offsetZ == 0) {
        int paddedX = (offsetX < 0 ? 0 : chunkSize + 1);
        ::gatherEastWestFace(neighbor, offsetX, chunkSize, paddedX, out);
    } else if (offsetX == 0 && offsetZ != 0) {
        int paddedZ = (offsetZ < 0 ? 0 : chunkSize + 1);
        ::gatherNorthSouthFace(neighbor, offsetZ, chunkSize, paddedZ, out);
    } else {
        int paddedX = (offsetX < 0 ? 0 : chunkSize + 1);
        int paddedZ = (offsetZ < 0 ? 0 : chunkSize + 1);
        ::gatherDiagonalXZ(neighbor, offsetX, offsetZ, chunkSize, paddedX,
                           paddedZ, out);
    }
}
} // namespace

namespace NeighborGatherer {

VoxelTypes::NeighborVoxelsMap gatherNeighborsForCoord(
    const ChunkCoord& center,
    const std::function<const RenderableChunk*(const ChunkCoord&)>& getChunk,
    int chunkSize) {
    VoxelTypes::NeighborVoxelsMap result;

    for (int offsetX = -1; offsetX <= 1; ++offsetX) {
        for (int offsetZ = -1; offsetZ <= 1; ++offsetZ) {
            if (offsetX == 0 && offsetZ == 0) {
                continue;
            }

            ChunkCoord neighborCoord{center.x + offsetX, center.z + offsetZ};
            const RenderableChunk* neighbor = getChunk(neighborCoord);
            gatherNeighborFaces(neighbor, offsetX, offsetZ, chunkSize, result);
        }
    }
    return result;
}

} // namespace NeighborGatherer