#include "CpuChunk.hpp"
#include "RenderableChunk.hpp"

CpuChunk::CpuChunk(int size, int worldX, int worldZ)
    : voxels(size, worldX, worldZ) {}

CpuChunk::CpuChunk(ChunkVoxels&& newVoxels) : voxels(std::move(newVoxels)) {}

bool CpuChunk::addCube(const glm::ivec3& p, CubeType t) {
    return voxels.addCube(p, t);
}
bool CpuChunk::removeCube(const glm::ivec3& p) { return voxels.removeCube(p); }
bool CpuChunk::isCubeInGrid(const glm::ivec3& p) const {
    return voxels.isCubeInGrid(p);
}
bool CpuChunk::isModified() const { return voxels.isModified(); }

CubeData CpuChunk::computeCubeData() { return voxels.computeCubeData(); }
void CpuChunk::applyCubeData(CubeData&& d) {
    voxels.storeCubes(std::move(d.cubes));
    voxels.setModified(false);
}

std::pair<glm::vec3, glm::vec3> CpuChunk::computeChunkAABB() const {
    return voxels.computeChunkAABB();
}
const auto& CpuChunk::getInstanceModelMatrices() const {
    return voxels.getInstanceModelMatrices();
}

std::unique_ptr<RenderableChunk> CpuChunk::toRenderable(
    unsigned vertexBufferObjects, unsigned ebo, unsigned webo) {
    return std::make_unique<RenderableChunk>(std::move(voxels),
                                             vertexBufferObjects, ebo, webo);
}
