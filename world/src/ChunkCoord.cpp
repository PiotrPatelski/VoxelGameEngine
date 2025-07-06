#include "ChunkCoord.hpp"

bool isPositionWithinBounds(const glm::ivec3& pos, int boundary) {
    return (pos.x >= 0 and pos.x < boundary and pos.y >= 0 and
            pos.y < boundary and pos.z >= 0 and pos.z < boundary);
}

bool isChunkWithinWindow(const ChunkCoord& coord, const ChunkWindow& window) {
    return (coord.x >= window.minX && coord.x <= window.maxX) &&
           (coord.z >= window.minZ && coord.z <= window.maxZ);
}

ChunkCoord fromWorldPosition(const glm::ivec3& position) {
    return {static_cast<int>(std::floor(position.x / 64)),
            static_cast<int>(std::floor(position.z / 64))};
}

glm::ivec3 toLocalPosition(const glm::ivec3& position) {
    return {static_cast<int>(std::floor(position.x)) % 64,
            static_cast<int>(std::floor(position.y)),
            static_cast<int>(std::floor(position.z)) % 64};
}