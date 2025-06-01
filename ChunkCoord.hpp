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

struct PositionXYHash {
    std::size_t operator()(const ChunkCoord& coord) const {
        std::size_t h1 = std::hash<int>{}(coord.x);
        std::size_t h2 = std::hash<int>{}(coord.z);
        return (h1 << 1) ^ h2;
    }
};

struct PositionXYZHash {
    std::size_t operator()(const glm::ivec3& v) const {
        std::size_t h1 = std::hash<int>()(v.x);
        std::size_t h2 = std::hash<int>()(v.y);
        std::size_t h3 = std::hash<int>()(v.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

bool isPositionWithinBounds(const glm::ivec3& pos, int boundary);

namespace Coord {
using CpuChunksMap =
    std::unordered_map<ChunkCoord, std::unique_ptr<CpuChunk>, PositionXYHash>;
using RenderableChunksMap =
    std::unordered_map<ChunkCoord, std::unique_ptr<RenderableChunk>,
                       PositionXYHash>;
using ChunkUpdatersMap =
    std::unordered_map<ChunkCoord, std::unique_ptr<ChunkUpdater>,
                       PositionXYHash>;
} // namespace Coord