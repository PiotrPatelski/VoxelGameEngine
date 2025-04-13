#include "TreeManager.hpp"
#include <cstdlib>
#include <functional>
#include <unordered_map>

TreeManager::TreeManager(int size) : chunkSize{size} {}

int TreeManager::findHighestVoxelY(
    const std::vector<std::vector<std::vector<bool>>>& voxelGrid, int x,
    int z) const {
    for (int y = chunkSize - 1; y >= 0; y--) {
        if (voxelGrid[x][z][y]) return y;
    }
    return -1;
}

void TreeManager::placeTreeTrunkAt(
    int x, int highestY, int z,
    std::vector<std::vector<std::vector<bool>>>& voxelGrid) {
    const int trunkHeight = 4 + (rand() % 4); // 4 to 7 cubes tall.
    for (int i = 1; i <= trunkHeight; i++) {
        const auto newY = highestY + i;
        if (newY < chunkSize) {
            glm::ivec3 pos{x, newY, z};
            trunkPositions.insert(pos);
            voxelGrid[x][z][newY] = true;
        }
    }
}

void TreeManager::generateCrownForTrunk(
    int colX, int colZ, int trunkTopY,
    std::vector<std::vector<std::vector<bool>>>& voxelGrid) {
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
                    crownPos.z < 0 or crownPos.z >= chunkSize)
                    continue;
                crownPositions.insert(crownPos);
                voxelGrid[crownPos.x][crownPos.z][crownPos.y] = true;
            }
        }
    }
}

void TreeManager::generateNewTreeTrunks(
    std::vector<std::vector<std::vector<bool>>>& voxelGrid) {
    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            const auto highestY = findHighestVoxelY(voxelGrid, x, z);
            if (highestY > 16) {
                const auto probability =
                    static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
                if (probability < 0.02f) { // 2% chance per column
                    placeTreeTrunkAt(x, highestY, z, voxelGrid);
                }
            }
        }
    }
}

TreeManager::Trunk TreeManager::buildTrunksMapping() const {
    Trunk trunkColumns(0, [](const std::pair<int, int>& col) -> std::size_t {
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

void TreeManager::generateCrownsForTrunks(
    const Trunk& trunkColumns,
    std::vector<std::vector<std::vector<bool>>>& voxelGrid) {
    for (const auto& [treeColumn, treeTrunkTopY] : trunkColumns) {
        const auto [treeColumnX, treeColumnZ] = treeColumn;
        bool crownAlreadyExists{false};
        for (const auto& crownPos : crownPositions) {
            if (crownPos.x == treeColumnX && crownPos.z == treeColumnZ) {
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

void TreeManager::generateTrees(
    std::vector<std::vector<std::vector<bool>>>& voxelGrid) {
    if (trunkPositions.empty()) {
        generateNewTreeTrunks(voxelGrid);
    }
    const auto trunks = buildTrunksMapping();
    generateCrownsForTrunks(trunks, voxelGrid);
}

void TreeManager::reapplyTrunks(
    float initialCubeX, float initialCubeZ,
    const std::function<void(const glm::vec3&, CubeType)>& createCubeCallback)
    const {
    for (const auto& pos : trunkPositions) {
        if (pos.y < chunkSize) {
            glm::vec3 worldPos{initialCubeX + static_cast<float>(pos.x),
                               static_cast<float>(pos.y),
                               initialCubeZ + static_cast<float>(pos.z)};
            createCubeCallback(worldPos, CubeType::LOG);
        }
    }
}

void TreeManager::reapplyCrowns(
    float initialCubeX, float initialCubeZ,
    const std::function<void(const glm::vec3&, CubeType)>& createCubeCallback)
    const {
    for (const auto& pos : crownPositions) {
        if (pos.y < chunkSize) {
            glm::vec3 worldPos{initialCubeX + static_cast<float>(pos.x),
                               static_cast<float>(pos.y),
                               initialCubeZ + static_cast<float>(pos.z)};
            createCubeCallback(worldPos, CubeType::LEAVES);
        }
    }
}

void TreeManager::removeTreeCubeAt(const glm::ivec3& localPos) {
    trunkPositions.erase(localPos);
    crownPositions.erase(localPos);
}
