#pragma once
#include <unordered_map>
#include <glm/vec3.hpp>
#include "ChunkCoord.hpp"
#include "Cube.hpp"

namespace VoxelTypes {
using LightGrid3D = std::vector<std::vector<std::vector<float>>>;
using VoxelGrid3D = std::vector<std::vector<std::vector<CubeType>>>;
using NeighborVoxelsMap =
    std::unordered_map<glm::ivec3, CubeType, PositionXYZHash>;
} // namespace VoxelTypes