#include "ChunkCoord.hpp"

bool isPositionWithinBounds(const glm::ivec3& pos, int boundary) {
    return (pos.x >= 0 and pos.x < boundary and pos.y >= 0 and
            pos.y < boundary and pos.z >= 0 and pos.z < boundary);
}