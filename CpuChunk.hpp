#pragma once

#include "ChunkVoxels.hpp"
#include "CubeData.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

class RenderableChunk;

class CpuChunk {
   public:
    CpuChunk(int size, int worldX, int worldZ);
    CpuChunk(ChunkVoxels&& voxels);
    CpuChunk(CpuChunk&&) = default;
    CpuChunk& operator=(CpuChunk&&) = default;
    CpuChunk(const CpuChunk&) = delete;
    CpuChunk& operator=(const CpuChunk&) = delete;
    ~CpuChunk() = default;

    bool addCube(const glm::ivec3& pos, CubeType type);
    bool removeCube(const glm::ivec3& pos);
    bool isCubeInGrid(const glm::ivec3& pos) const;
    bool isModified() const;

    CubeData computeCubeData();
    void applyCubeData(CubeData&& data);

    std::pair<glm::vec3, glm::vec3> computeChunkAABB() const;
    const auto& getInstanceModelMatrices() const;

    std::unique_ptr<RenderableChunk> toRenderable(unsigned sharedVBO,
                                                  unsigned sharedCubeEBO,
                                                  unsigned sharedWaterEBO);

   private:
    ChunkVoxels voxels;
};