#pragma once

#include <vector>
#include <queue>
#include <unordered_map>
#include <glm/vec3.hpp>
#include "VoxelTypes.hpp"

class LightPropagator {
   public:
    LightPropagator(int chunkSize, float attenuationFactor);

    LightPropagator(const LightPropagator&) = delete;
    LightPropagator& operator=(const LightPropagator&) = delete;
    LightPropagator(LightPropagator&&) = delete;
    LightPropagator& operator=(LightPropagator&&) = delete;

    std::vector<float> computeLightMask(
        const VoxelTypes::VoxelGrid3D& originalVoxelGrid,
        const std::vector<glm::ivec3>& torchPositions,
        const VoxelTypes::NeighborVoxelsMap& neighborsSurroundingCubes);

   private:
    int originalSize{0};
    int paddedSize{0};
    float attenuation{0.f};

    VoxelTypes::VoxelGrid3D paddedVoxels;
    VoxelTypes::LightGrid3D paddedLight;
    std::queue<glm::ivec3> bfsQueue;

    void clearPaddedGrids();
    void emplaceChunkIntoPaddedGrid(const VoxelTypes::VoxelGrid3D& original);
    void insertNeighborsSurroundingCubes(
        const VoxelTypes::NeighborVoxelsMap& halo);
    void seedChunkInternalTorches(const std::vector<glm::ivec3>& torches);
    void runPropagation();
    void propagateToNeighbors(const glm::ivec3& position, float nextLightValue);
    std::vector<float> extractFinalChunkWithSeededLight();
};
