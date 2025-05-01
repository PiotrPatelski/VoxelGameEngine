#pragma once
#include <unordered_set>
#include <functional>
#include <glm/vec3.hpp>
#include "Cube.hpp"

struct PositionHash {
    std::size_t operator()(const glm::ivec3& v) const {
        std::size_t h1 = std::hash<int>()(v.x);
        std::size_t h2 = std::hash<int>()(v.y);
        std::size_t h3 = std::hash<int>()(v.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

class TreeGenerator {
   public:
    using VoxelGrid = std::vector<std::vector<std::vector<CubeType>>>;
    using CubeCreator = std::function<void(const glm::ivec3&, CubeType)>;
    explicit TreeGenerator(int chunkSize);
    ~TreeGenerator() = default;
    TreeGenerator(const TreeGenerator&) = delete;
    TreeGenerator(TreeGenerator&&) noexcept = default;
    TreeGenerator& operator=(const TreeGenerator&) = delete;
    TreeGenerator& operator=(TreeGenerator&&) noexcept = default;
    void generateTrees(VoxelGrid& voxelGrid);

    void reapplyTrunks(float initialCubeX, float initialCubeZ,
                       const CubeCreator& createCubeCallback) const;

    void reapplyCrowns(float initialCubeX, float initialCubeZ,
                       const CubeCreator& createCubeCallback) const;

    void removeTreeCubeAt(const glm::ivec3& localPos);

   private:
    using Trunk = std::unordered_map<
        std::pair<int, int>, int,
        std::function<std::size_t(const std::pair<int, int>&)>>;
    int findHighestFilledVoxelY(const VoxelGrid& voxelGrid, int x, int z) const;
    void placeTreeTrunkAt(int x, int highestY, int z, VoxelGrid& voxelGrid);
    void generateNewTreeTrunks(VoxelGrid& voxelGrid);
    void generateCrownForTrunk(int colX, int colZ, int trunkTopY,
                               VoxelGrid& voxelGrid);
    void generateCrownsForTrunks(const Trunk& trunkColumns,
                                 VoxelGrid& voxelGrid);
    Trunk buildTrunksMapping() const;
    int chunkSize{};
    std::unordered_set<glm::ivec3, PositionHash> trunkPositions{};
    std::unordered_set<glm::ivec3, PositionHash> crownPositions{};
};