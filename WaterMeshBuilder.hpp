#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Cube.hpp"
#include "VoxelTypes.hpp"
#include "VertexData.hpp"

class WaterMeshBuilder {
   public:
    struct WaterSurface {
        std::vector<Vertex> vertices{};
        std::vector<unsigned int> indices{};
        CubeType waterType;
        glm::vec3 center{};
    };

    std::vector<WaterSurface> buildWaterSurfaces(
        const VoxelTypes::VoxelGrid3D& voxelGrid, float chunkWorldX,
        float chunkWorldZ);

   private:
    std::vector<std::vector<std::vector<bool>>> processed;

    WaterSurface buildConnectedSurface(const VoxelTypes::VoxelGrid3D& voxelGrid,
                                       const glm::ivec3& position,
                                       float chunkWorldX, float chunkWorldZ);

    void initializeProcessedGrid(int chunkSize);

    std::vector<WaterSurface> collectWaterSurfaces(
        const VoxelTypes::VoxelGrid3D& voxelGrid, int chunkSize,
        float chunkWorldX, float chunkWorldZ);

    bool isValidWaterPosition(const VoxelTypes::VoxelGrid3D& voxelGrid,
                              const glm::ivec3& position) const;

    std::vector<glm::ivec3> createWaterSurface(
        const VoxelTypes::VoxelGrid3D& voxelGrid, const glm::ivec3& position,
        CubeType waterType);

    void generateUnifiedMesh(const std::vector<glm::ivec3>& surfaceBlocks,
                             float chunkWorldX, float chunkWorldZ,
                             WaterSurface& surface);

    glm::vec3 calculateSurfaceCenter(
        const std::vector<glm::ivec3>& surfaceBlocks, float chunkWorldX,
        float chunkWorldZ) const;

    glm::vec3 calculateWorldPosition(const glm::ivec3& block, float chunkWorldX,
                                     float chunkWorldZ) const;

    std::vector<Vertex> createWaterQuadVertices(
        const glm::vec3& position) const;

    void addWaterQuad(const glm::vec3& position, WaterSurface& surface);
};
