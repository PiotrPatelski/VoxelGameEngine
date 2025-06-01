#pragma once
#include "glm/glm.hpp"
#include <array>
#include <memory>
#include <unordered_map>

class CpuChunk;
class RenderableChunk;
class ChunkUpdater;

constexpr std::array<glm::ivec3, 6> NEIGHBOR_OFFSETS = {
    glm::ivec3(1, 0, 0),  glm::ivec3(-1, 0, 0), glm::ivec3(0, 1, 0),
    glm::ivec3(0, -1, 0), glm::ivec3(0, 0, 1),  glm::ivec3(0, 0, -1)};

struct ChunkCoord {
    int x{0};
    int z{0};
    bool operator==(const ChunkCoord& other) const {
        return x == other.x and z == other.z;
    }
};

namespace std {
template <>
struct hash<ChunkCoord> {
    std::size_t operator()(const ChunkCoord& coord) const noexcept {
        std::size_t h1 = std::hash<int>{}(coord.x);
        std::size_t h2 = std::hash<int>{}(coord.z);
        return (h1 << 1) ^ h2;
    }
};
} // namespace std