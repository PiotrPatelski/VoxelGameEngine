#include "TreeGenerator.hpp"
#include <cstdlib>
#include <functional>
#include <unordered_map>

TreeGenerator::TreeGenerator(int size) : chunkSize{size} {}

int TreeGenerator::findHighestFilledVoxelY(
    const VoxelTypes::VoxelGrid3D& voxelGrid, int x, int z) const {
    for (int y = chunkSize - 1; y >= 0; y--) {
        if (voxelGrid[x][z][y] != CubeType::NONE) return y;
    }
    return -1;
}

void TreeGenerator::placeTreeTrunkAt(int x, int highestFilledY, int z,
                                     VoxelTypes::VoxelGrid3D& voxelGrid) {
    int trunkBaseY = highestFilledY + 1;
    const int trunkHeight = 4 + (rand() % 4);
    for (int i = 0; i < trunkHeight; i++) {
        int newY = trunkBaseY + i;
        if (newY < chunkSize) {
            glm::ivec3 pos{x, newY, z};
            trunkPositions.insert(pos);
            voxelGrid[x][z][newY] = CubeType::LOG;
        }
    }
}

void TreeGenerator::generateCrownForTrunk(int colX, int colZ, int trunkTopY,
                                          VoxelTypes::VoxelGrid3D& voxelGrid) {
    for (int offsetX = -2; offsetX <= 2; offsetX++) {
        for (int offsetY = -2; offsetY <= 2; offsetY++) {
            for (int offsetZ = -2; offsetZ <= 2; offsetZ++) {
                if (offsetX == 0 and offsetY <= 0 and offsetZ == 0) continue;
                const auto squaredDistance =
                    offsetX * offsetX + offsetY * offsetY + offsetZ * offsetZ;
                if (squaredDistance <= 1 or squaredDistance > 4) continue;
                glm::ivec3 crownPos{colX + offsetX, trunkTopY + offsetY,
                                    colZ + offsetZ};
                if (crownPos.x < 0 or crownPos.x >= chunkSize or
                    crownPos.y < 0 or crownPos.y >= chunkSize or
                    crownPos.z < 0 or crownPos.z >= chunkSize) {
                    continue;
                }
                crownPositions.insert(crownPos);
                voxelGrid[crownPos.x][crownPos.z][crownPos.y] =
                    CubeType::LEAVES;
            }
        }
    }
}

void TreeGenerator::generateNewTreeTrunks(VoxelTypes::VoxelGrid3D& voxelGrid) {
    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            const auto highestY = findHighestFilledVoxelY(voxelGrid, x, z);
            if (highestY > 16) {
                const auto probability =
                    static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
                if (probability < 0.02f) {
                    placeTreeTrunkAt(x, highestY, z, voxelGrid);
                }
            }
        }
    }
}

TreeGenerator::Trunk TreeGenerator::buildTrunksMapping() const {
    Trunk trunkColumns(0, [](const std::pair<int, int>& col) {
        return std::hash<int>()(col.first) ^
               (std::hash<int>()(col.second) << 1);
    });
    for (const auto& pos : trunkPositions) {
        std::pair<int, int> column{pos.x, pos.z};
        if (trunkColumns.find(column) == trunkColumns.end() or
            pos.y > trunkColumns[column]) {
            trunkColumns[column] = pos.y;
        }
    }
    return trunkColumns;
}

void TreeGenerator::generateCrownsForTrunks(
    const Trunk& trunkColumns, VoxelTypes::VoxelGrid3D& voxelGrid) {
    for (const auto& [treeColumn, treeTrunkTopY] : trunkColumns) {
        const auto [treeColumnX, treeColumnZ] = treeColumn;
        bool crownAlreadyExists{false};
        for (const auto& crownPos : crownPositions) {
            if (crownPos.x == treeColumnX and crownPos.z == treeColumnZ) {
                crownAlreadyExists = true;
                break;
            }
        }
        if (not crownAlreadyExists) {
            generateCrownForTrunk(treeColumnX, treeColumnZ, treeTrunkTopY,
                                  voxelGrid);
        }
    }
}

void TreeGenerator::generateTrees(VoxelTypes::VoxelGrid3D& voxelGrid) {
    if (trunkPositions.empty()) {
        generateNewTreeTrunks(voxelGrid);
    }
    const auto trunks = buildTrunksMapping();
    generateCrownsForTrunks(trunks, voxelGrid);
}

void TreeGenerator::reapplyTrunks(
    float initialCubeX, float initialCubeZ,
    const std::function<void(const glm::ivec3&, CubeType)>& createCubeCallback)
    const {
    for (const auto& pos : trunkPositions) {
        if (pos.y < chunkSize) {
            const glm::ivec3 worldPos{initialCubeX + pos.x, pos.y,
                                      initialCubeZ + pos.z};
            createCubeCallback(worldPos, CubeType::LOG);
        }
    }
}

void TreeGenerator::reapplyCrowns(
    float initialCubeX, float initialCubeZ,
    const std::function<void(const glm::ivec3&, CubeType)>& createCubeCallback)
    const {
    for (const auto& pos : crownPositions) {
        if (pos.y < chunkSize) {
            const glm::ivec3 worldPos{initialCubeX + pos.x, pos.y,
                                      initialCubeZ + pos.z};
            createCubeCallback(worldPos, CubeType::LEAVES);
        }
    }
}

void TreeGenerator::removeTreeCubeAt(const glm::ivec3& localPos) {
    trunkPositions.erase(localPos);
    crownPositions.erase(localPos);
}
