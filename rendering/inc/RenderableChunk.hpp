#pragma once

#include "ChunkVoxels.hpp"
#include "ChunkGraphics.hpp"
#include "CubeData.hpp"
#include "Frustum.hpp"
#include "VoxelTypes.hpp"
#include "RenderableWaterMesh.hpp"

class CpuChunk;

class RenderableChunk {
   public:
    RenderableChunk(ChunkVoxels&& voxelsData, unsigned sharedVBO,
                    unsigned sharedCubeEBO, unsigned sharedWaterEBO);
    ~RenderableChunk() = default;

    RenderableChunk(const RenderableChunk&) = delete;
    RenderableChunk& operator=(const RenderableChunk&) = delete;
    RenderableChunk(RenderableChunk&&) = delete;
    RenderableChunk& operator=(RenderableChunk&&) = delete;
    std::unique_ptr<CpuChunk> toCpuChunk();
    bool addCube(const glm::ivec3& localPos, CubeType type);
    bool removeCube(const glm::ivec3& localPos);
    bool isCubeInGrid(const glm::ivec3& localPos) const;
    bool isModified() const;
    bool isValidCubeAt(const glm::ivec3& pos) const;
    CubeType getCubeType(const glm::ivec3& pos) const;
    void markModified();
    void setNeighborsSurroundingCubes(VoxelTypes::NeighborVoxelsMap&& data);
    void clearNeighborsSurroundingCubes();
    glm::vec3 getChunkCenter() const;
    CubeData computeCubeData();
    void applyCubeData(CubeData&& data);
    void createWaterMeshes();
    void renderByType(Shader& shader, CubeType type);
    void renderWaterMeshes(Shader& shader);
    void performFrustumCulling(const Frustum& frustum);

   private:
    ChunkVoxels voxels;
    ChunkGraphics graphics;
    std::vector<RenderableWaterMesh> waterMeshes{};
    bool isCulled{false};
};
