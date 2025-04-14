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

class TreeManager {
   public:
    explicit TreeManager(int chunkSize);
    ~TreeManager() = default;
    TreeManager(const TreeManager&) = delete;
    TreeManager(TreeManager&&) = delete;
    TreeManager& operator=(const TreeManager&) = delete;
    TreeManager& operator=(TreeManager&&) = delete;
    void generateTrees(
        std::vector<std::vector<std::vector<CubeType>>>& voxelGrid);

    void reapplyTrunks(float initialCubeX, float initialCubeZ,
                       const std::function<void(const glm::vec3&, CubeType)>&
                           createCubeCallback) const;

    void reapplyCrowns(float initialCubeX, float initialCubeZ,
                       const std::function<void(const glm::vec3&, CubeType)>&
                           createCubeCallback) const;

    void removeTreeCubeAt(const glm::ivec3& localPos);

   private:
    using Trunk = std::unordered_map<
        std::pair<int, int>, int,
        std::function<std::size_t(const std::pair<int, int>&)>>;
    int findHighestFilledVoxelY(
        const std::vector<std::vector<std::vector<CubeType>>>& voxelGrid, int x,
        int z) const;
    void placeTreeTrunkAt(
        int x, int highestY, int z,
        std::vector<std::vector<std::vector<CubeType>>>& voxelGrid);
    void generateNewTreeTrunks(
        std::vector<std::vector<std::vector<CubeType>>>& voxelGrid);
    void generateCrownForTrunk(
        int colX, int colZ, int trunkTopY,
        std::vector<std::vector<std::vector<CubeType>>>& voxelGrid);
    void generateCrownsForTrunks(
        const Trunk& trunkColumns,
        std::vector<std::vector<std::vector<CubeType>>>& voxelGrid);
    Trunk buildTrunksMapping() const;
    const int chunkSize{};
    std::unordered_set<glm::ivec3, PositionHash> trunkPositions{};
    std::unordered_set<glm::ivec3, PositionHash> crownPositions{};
};