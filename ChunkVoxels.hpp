#pragma once
#include <mutex>
#include <vector>
#include <memory>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>
#include "GridGenerator.hpp"
#include "TreeGenerator.hpp"
#include "CubeData.hpp"

class ChunkVoxels {
   public:
    using VoxelGrid = GridGenerator::VoxelGrid;
    ChunkVoxels(int size, int worldX, int worldZ);
    ChunkVoxels(const ChunkVoxels&) = delete;
    ChunkVoxels& operator=(const ChunkVoxels&) = delete;
    ChunkVoxels(ChunkVoxels&& other) noexcept;
    ChunkVoxels& operator=(ChunkVoxels&& other) noexcept;

    inline const std::unordered_map<CubeType, std::vector<glm::mat4>>&
    getInstanceModelMatrices() const {
        return instanceModelMatrices;
    }

    inline glm::vec3 getChunkOrigin() const {
        return glm::vec3(chunkWorldXIndex * size, 0.0f,
                         chunkWorldZIndex * size);
    }
    inline int getSize() const { return size; }

    bool addCube(const glm::ivec3& localPos, CubeType type);
    bool removeCube(const glm::ivec3& localPos);
    bool isCubeInGrid(const glm::ivec3& localPos) const;

    bool isModified() const { return modified; }
    void setModified(bool value) { modified = value; }

    CubeData computeCubeData();
    void storeCubes(std::vector<std::unique_ptr<Cube>>&& newCubes);

    std::pair<glm::vec3, glm::vec3> computeChunkAABB() const;

   private:
    using CubeCreator = std::function<void(const glm::ivec3&, CubeType)>;

    VoxelGrid generateInitialVoxelGrid();
    void createCube(const glm::ivec3& worldPos, CubeType type);
    void processVoxelGrid(float firstCubeX, float firstCubeZ,
                          const CubeCreator& action);
    void regenerateChunk(const CubeCreator& action);
    void rebuildCubesFromGrid();

    int size;
    int chunkWorldXIndex, chunkWorldZIndex;
    static constexpr int waterHeight{14};

    TreeGenerator treeGenerator;
    VoxelGrid voxelGrid{};
    std::vector<std::unique_ptr<Cube>> cubes{};
    std::unordered_map<CubeType, std::vector<glm::mat4>>
        instanceModelMatrices{};
    std::vector<glm::ivec3> torchPositions{};

    bool modified{true};
    mutable std::mutex voxelMutex;
};
