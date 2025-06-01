#pragma once
#include <unordered_set>
#include <functional>
#include <glm/vec3.hpp>
#include "Cube.hpp"
#include "ChunkCoord.hpp"
#include "VoxelTypes.hpp"

class TreeGenerator {
   public:
    using CubeCreator = std::function<void(const glm::ivec3&, CubeType)>;
    explicit TreeGenerator(int chunkSize);
    ~TreeGenerator() = default;
    TreeGenerator(const TreeGenerator&) = delete;
    TreeGenerator(TreeGenerator&&) noexcept = default;
    TreeGenerator& operator=(const TreeGenerator&) = delete;
    TreeGenerator& operator=(TreeGenerator&&) noexcept = default;
    void generateTrees(VoxelTypes::VoxelGrid3D& voxelGrid);

    void reapplyTrunks(float initialCubeX, float initialCubeZ,
                       const CubeCreator& createCubeCallback) const;

    void reapplyCrowns(float initialCubeX, float initialCubeZ,
                       const CubeCreator& createCubeCallback) const;

    void removeTreeCubeAt(const glm::ivec3& localPos);

   private:
    using Trunk = std::unordered_map<
        std::pair<int, int>, int,
        std::function<std::size_t(const std::pair<int, int>&)>>;
    int findHighestFilledVoxelY(const VoxelTypes::VoxelGrid3D& voxelGrid, int x,
                                int z) const;
    void placeTreeTrunkAt(int x, int highestY, int z,
                          VoxelTypes::VoxelGrid3D& voxelGrid);
    void generateNewTreeTrunks(VoxelTypes::VoxelGrid3D& voxelGrid);
    void generateCrownForTrunk(int colX, int colZ, int trunkTopY,
                               VoxelTypes::VoxelGrid3D& voxelGrid);
    void generateCrownsForTrunks(const Trunk& trunkColumns,
                                 VoxelTypes::VoxelGrid3D& voxelGrid);
    Trunk buildTrunksMapping() const;
    int chunkSize{};
    std::unordered_set<glm::ivec3, PositionHash> trunkPositions{};
    std::unordered_set<glm::ivec3, PositionHash> crownPositions{};
};