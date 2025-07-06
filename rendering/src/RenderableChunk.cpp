#include "RenderableChunk.hpp"
#include "CpuChunk.hpp"

RenderableChunk::RenderableChunk(ChunkVoxels&& voxelsData, unsigned sharedVBO,
                                 unsigned sharedCubeEBO,
                                 unsigned sharedWaterEBO)
    : voxels(std::move(voxelsData)) {
    graphics.initializeGL(sharedVBO, sharedCubeEBO, sharedWaterEBO,
                          voxels.getSize());

    createWaterMeshes();
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

bool RenderableChunk::isValidCubeAt(const glm::ivec3& position) const {
    const int size = voxels.getSize();
    if (position.x < 0 or position.x >= size or position.y < 0 or
        position.y >= size or position.z < 0 or position.z >= size) {
        return false;
    }
    return voxels.isCubeInGrid(position);
}

CubeType RenderableChunk::getCubeType(const glm::ivec3& position) const {
    return voxels.getCubeTypeAt(position);
}

CubeData RenderableChunk::computeCubeData() { return voxels.computeCubeData(); }

std::unique_ptr<CpuChunk> RenderableChunk::toCpuChunk() {
    return std::make_unique<CpuChunk>(std::move(voxels));
}

void RenderableChunk::markModified() { voxels.setModified(true); }

void RenderableChunk::setNeighborsSurroundingCubes(
    VoxelTypes::NeighborVoxelsMap&& data) {
    voxels.setNeighborsSurroundingCubes(std::move(data));
}

void RenderableChunk::clearNeighborsSurroundingCubes() {
    voxels.clearNeighborsSurroundingCubes();
}

glm::vec3 RenderableChunk::getChunkCenter() const {
    return voxels.getChunkOrigin() + glm::vec3(voxels.getSize() / 2.0f);
}

void RenderableChunk::applyCubeData(CubeData&& data) {
    voxels.storeCubes(std::move(data.cubes));
    graphics.updateInstanceData(data.instanceModelMatrices);
    graphics.updateLightVolume(data.lightVolume, voxels.getSize());
    voxels.setModified(false);
    createWaterMeshes();
}

void RenderableChunk::createWaterMeshes() {
    waterMeshes.clear();
    const auto& waterMeshData = voxels.getWaterMeshData();
    waterMeshes.reserve(waterMeshData.size());
    for (const auto& data : waterMeshData) {
        if (not data.isEmpty()) {
            waterMeshes.emplace_back(data);
        }
    }
}

void RenderableChunk::renderByType(Shader& shader, CubeType type) {
    if (not isCulled) {
        shader.setVec3("chunkOrigin", voxels.getChunkOrigin());
        shader.setFloat("chunkSize", float(voxels.getSize()));
        graphics.renderByType(type);
    }
}

void RenderableChunk::renderWaterMeshes(Shader& shader) {
    if (not isCulled) {
        shader.setVec3("chunkOrigin", voxels.getChunkOrigin());
        shader.setFloat("chunkSize", float(voxels.getSize()));
        for (const auto& waterMesh : waterMeshes) {
            if (not waterMesh.isEmpty()) {
                waterMesh.render();
            }
        }
    }
}

void RenderableChunk::performFrustumCulling(const Frustum& frustum) {
    auto [min, max] = voxels.computeChunkAABB();
    isCulled = !frustum.isAABBInside(min, max);
}
