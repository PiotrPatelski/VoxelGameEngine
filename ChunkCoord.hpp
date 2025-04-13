#pragma once
#include "glm/glm.hpp"

struct ChunkCoord {
    int x;
    int z;
    bool operator==(const ChunkCoord& other) const {
        return x == other.x && z == other.z;
    }
};

namespace std {
template <>
struct hash<ChunkCoord> {
    std::size_t operator()(const ChunkCoord& coord) const noexcept {
        // Combine the two ints into one hash value.
        std::size_t h1 = std::hash<int>{}(coord.x);
        std::size_t h2 = std::hash<int>{}(coord.z);
        return (h1 << 1) ^ h2;
    }
};
} // namespace std