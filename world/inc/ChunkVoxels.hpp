#pragma once
#include <mutex>
#include <vector>
#include <memory>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>
#include "GridGenerator.hpp"
#include "TreeGenerator.hpp"
#include "CubeData.hpp"
#include "VoxelTypes.hpp"
#include "RenderableWaterMesh.hpp"
#include "CpuWaterMesh.hpp"
#include "WaterMeshBuilder.hpp"

class ChunkVoxels {
   public:
    ChunkVoxels(int size, int worldX, int worldZ);
    ChunkVoxels(const ChunkVoxels&) = delete;
    ChunkVoxels& operator=(const ChunkVoxels&) = delete;
    ChunkVoxels(ChunkVoxels&& other) noexcept;
    ChunkVoxels& operator=(ChunkVoxels&& other) noexcept;

    inline int getSize() const { return size; }
    inline bool isModified() const { return modified; }

    bool addCube(const glm::ivec3& localPos, CubeType type);
    bool removeCube(const glm::ivec3& localPos);
    bool isCubeInGrid(const glm::ivec3& localPos) const;

    const std::unordered_map<CubeType, std::vector<glm::mat4>>&
    getInstanceModelMatrices() const;
    glm::vec3 getChunkOrigin() const;

    CubeType getCubeTypeAt(const glm::ivec3& localPos) const;
    CubeData computeCubeData();
    std::pair<glm::vec3, glm::vec3> computeChunkAABB() const;

    void setModified(bool value);
    void setNeighborsSurroundingCubes(VoxelTypes::NeighborVoxelsMap&& data);
    void clearNeighborsSurroundingCubes();
    void storeCubes(std::vector<std::unique_ptr<Cube>>&& newCubes);

    const std::vector<CpuWaterMesh>& getWaterMeshData() const {
        return waterMeshData;
    }
    void buildWaterMeshData();

   private:
    using CubeCreator = std::function<void(const glm::ivec3&, CubeType)>;

    VoxelTypes::VoxelGrid3D generateInitialVoxelGrid();
    void createCube(const glm::ivec3& worldPos, CubeType type);
    void processVoxelGrid(float firstCubeX, float firstCubeZ,
                          const CubeCreator& action);
    void regenerateChunk(const CubeCreator& action);
    void rebuildCubesFromGrid();

    void placeWaterBlocks();
    glm::vec2 computeChunkWorldPosition() const;
    void storeValidWaterSurfaces(
        const std::vector<WaterMeshBuilder::WaterSurface>& surfaces);

    int size{0};
    int chunkWorldXIndex{0};
    int chunkWorldZIndex{0};

    TreeGenerator treeGenerator;
    VoxelTypes::VoxelGrid3D voxelGrid{};
    std::vector<std::unique_ptr<Cube>> cubes{};
    std::unordered_map<CubeType, std::vector<glm::mat4>>
        instanceModelMatrices{};
    std::vector<glm::ivec3> torchPositions{};
    VoxelTypes::NeighborVoxelsMap neighborsSurroundingCubes{};

    std::vector<CpuWaterMesh> waterMeshData{};

    bool modified{true};
    mutable std::mutex voxelMutex;
};
