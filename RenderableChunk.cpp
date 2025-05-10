#include "RenderableChunk.hpp"
#include "CpuChunk.hpp"

RenderableChunk::RenderableChunk(ChunkVoxels&& voxelsData, unsigned sharedVBO,
                                 unsigned sharedCubeEBO,
                                 unsigned sharedWaterEBO)
    : voxels(std::move(voxelsData)) {
    graphics.initializeGL(sharedVBO, sharedCubeEBO, sharedWaterEBO,
                          voxels.getSize());
}

bool RenderableChunk::addCube(const glm::ivec3& position, CubeType type) {
    return voxels.addCube(position, type);
}
bool RenderableChunk::removeCube(const glm::ivec3& position) {
    return voxels.removeCube(position);
}
bool RenderableChunk::isCubeInGrid(const glm::ivec3& position) const {
    return voxels.isCubeInGrid(position);
}
bool RenderableChunk::isModified() const { return voxels.isModified(); }

CubeData RenderableChunk::computeCubeData() { return voxels.computeCubeData(); }

std::unique_ptr<CpuChunk> RenderableChunk::toCpuChunk() {
    return std::make_unique<CpuChunk>(std::move(voxels));
}

void RenderableChunk::applyCubeData(CubeData&& data) {
    voxels.storeCubes(std::move(data.cubes));
    graphics.updateInstanceData(data.instanceModelMatrices);
    graphics.updateLightVolume(data.lightVolume, voxels.getSize());
    voxels.setModified(false);
}

void RenderableChunk::renderByType(Shader& shader, CubeType type) {
    if (not isCulled) {
        shader.setVec3("chunkOrigin", voxels.getChunkOrigin());
        shader.setFloat("chunkSize", float(voxels.getSize()));
        graphics.renderByType(type);
    }
}

void RenderableChunk::performFrustumCulling(const Frustum& frustum) {
    auto [min, max] = voxels.computeChunkAABB();
    isCulled = !frustum.isAABBInside(min, max);
}
