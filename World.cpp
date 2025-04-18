#include "World.hpp"
#include "Raycaster.hpp"
#include "VertexData.hpp"
#include <cstdio>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

void World::loadInitialChunks() {
    std::lock_guard<std::mutex> lock(loadedChunksMutex);
    // Initially load full GL+CPU chunks around (0,0)
    for (int x = -renderDistance; x <= renderDistance; ++x) {
        for (int z = -renderDistance; z <= renderDistance; ++z) {
            ChunkCoord coord{x, z};
            auto chunk = chunkLoader->createChunk(x, z);
            chunk->initializeGL(chunkLoader->getSharedVBO(),     // ← added
                                chunkLoader->getSharedEBO(),     // ← added
                                chunkLoader->getSharedWaterEBO() // ← added
            );
            loadedChunks[coord] = std::move(chunk);
            chunkUpdaters[coord] =
                std::make_unique<ChunkUpdater>(loadedChunks[coord].get());
        }
    }
}

World::World() {
    printf("World::Init!\n");
    chunkLoader = std::make_unique<ChunkLoader>(renderDistance, chunkSize);
    loadInitialChunks();
    lastCameraChunk = {0, 0};
}

bool World::addCubeFromRaycast(const Camera& camera, float maxDistance,
                               CubeType type) {
    const auto hitOpt =
        Raycaster{camera, chunkSize}.raycast(loadedChunks, maxDistance);
    if (hitOpt.has_value()) {
        const auto hit = hitOpt.value();
        const auto newCubePos = hit.position + hit.normal;
        const auto chunkX = floorDivide(newCubePos.x, chunkSize);
        const auto chunkZ = floorDivide(newCubePos.z, chunkSize);
        Chunk* chunk = getChunk({chunkX, chunkZ});
        if (chunk) {
            const auto localX = negativeSafeModulo(newCubePos.x, chunkSize);
            const auto localZ = negativeSafeModulo(newCubePos.z, chunkSize);
            const auto localY = newCubePos.y;
            return chunk->addCube(glm::ivec3(localX, localY, localZ), type);
        }
    }
    return false;
}

bool World::removeCubeFromRaycast(const Camera& camera, float maxDistance) {
    const auto hitOpt =
        Raycaster{camera, chunkSize}.raycast(loadedChunks, maxDistance);
    if (hitOpt.has_value()) {
        const auto hit = hitOpt.value();
        const auto worldCubePos = hit.position;
        const auto chunkX = floorDivide(worldCubePos.x, chunkSize);
        const auto chunkZ = floorDivide(worldCubePos.z, chunkSize);
        Chunk* chunk = getChunk({chunkX, chunkZ});
        if (chunk) {
            const auto localX = negativeSafeModulo(worldCubePos.x, chunkSize);
            const auto localZ = negativeSafeModulo(worldCubePos.z, chunkSize);
            const auto localY = worldCubePos.y;
            return chunk->removeCube(glm::ivec3(localX, localY, localZ));
        }
    }
    return false;
}

Chunk* World::getChunk(const ChunkCoord& coord) {
    std::lock_guard<std::mutex> lock(loadedChunksMutex);
    const auto it = loadedChunks.find(coord);
    return (it != loadedChunks.end()) ? it->second.get() : nullptr;
}

bool World::updateCameraChunk(const ChunkCoord& currentCamCoord) {
    std::lock_guard<std::mutex> lock(loadedChunksMutex);
    if (currentCamCoord != lastCameraChunk) {
        lastCameraChunk = currentCamCoord;
        return true;
    }
    return false;
}

std::unordered_set<ChunkCoord> World::getLoadedChunkKeys() {
    std::unordered_set<ChunkCoord> keys;
    std::lock_guard<std::mutex> lock(loadedChunksMutex);
    for (const auto& [chunkCoord, _] : loadedChunks) {
        keys.insert(chunkCoord);
    }
    return keys;
}

void World::mergeNewChunks(
    std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>>& newChunks) {
    std::lock_guard<std::mutex> lock(loadedChunksMutex);
    for (auto& [coord, chunkPtr] : newChunks) {
        auto [it, inserted] =
            loadedChunks.try_emplace(coord, nullptr); // ← modified
        if (inserted) {
            it->second = std::move(chunkPtr);
            it->second->initializeGL(chunkLoader->getSharedVBO(), // ← added
                                     chunkLoader->getSharedEBO(), // ← added
                                     chunkLoader->getSharedWaterEBO() // ← added
            );
            chunkUpdaters[coord] =
                std::make_unique<ChunkUpdater>(it->second.get());
        }
    }
}

void World::reloadCurrentlyRelevantChunkGroup(
    const ChunkCoord& currentCamCoord) {
    const auto isCameraMoved = updateCameraChunk(currentCamCoord);
    const auto existingKeys = getLoadedChunkKeys();
    if (chunkLoader->isTaskRunning() and chunkLoader->isFinished()) {
        auto newChunks = chunkLoader->retrieveNewChunks();
        mergeNewChunks(newChunks);
    }
    if (isCameraMoved and not chunkLoader->isTaskRunning()) {
        chunkLoader->launchTask(currentCamCoord.x, currentCamCoord.z,
                                existingKeys);
    }
}

void World::adjustLoadedChunks(const ChunkCoord& currentCamCoord) {
    int minX = currentCamCoord.x - renderDistance;
    int maxX = currentCamCoord.x + renderDistance;
    int minZ = currentCamCoord.z - renderDistance;
    int maxZ = currentCamCoord.z + renderDistance;

    std::lock_guard<std::mutex> lock(loadedChunksMutex);
    // Restore any savedChunks that re-entered range
    for (auto it = savedChunks.begin(); it != savedChunks.end();) {
        const auto& coord = it->first;
        if (coord.x >= minX && coord.x <= maxX && coord.z >= minZ &&
            coord.z <= maxZ) {
            auto chunk = std::move(it->second);
            chunk->initializeGL(chunkLoader->getSharedVBO(),     // ← added
                                chunkLoader->getSharedEBO(),     // ← added
                                chunkLoader->getSharedWaterEBO() // ← added
            );
            loadedChunks[coord] = std::move(chunk);
            chunkUpdaters[coord] =
                std::make_unique<ChunkUpdater>(loadedChunks[coord].get());
            it = savedChunks.erase(it);
        } else {
            ++it;
        }
    }

    // Evict out‐of‐range chunks
    for (auto it = loadedChunks.begin(); it != loadedChunks.end();) {
        const auto& coord = it->first;
        if (coord.x < minX || coord.x > maxX || coord.z < minZ ||
            coord.z > maxZ) {
            auto upIt = chunkUpdaters.find(coord);
            if (upIt != chunkUpdaters.end() &&
                upIt->second->isUpdateRunning()) {
                ++it; // wait for in‐flight updater
                continue;
            }
            savedChunks[coord] = std::move(it->second);
            if (upIt != chunkUpdaters.end()) chunkUpdaters.erase(upIt);
            it = loadedChunks.erase(it);
        } else {
            ++it;
        }
    }
}

void World::runUpdatePerChunk() {
    for (auto& [coord, updater] : chunkUpdaters) {
        Chunk* chunk = getChunk(coord);
        if (chunk && chunk->isModified() && !updater->isUpdateRunning()) {
            updater->launchUpdate();
        }
        updater->checkAndApplyUpdate();
    }
}

void World::updateLoadedChunks(const glm::vec3& camPos) {
    const auto camChunkX = static_cast<int>(std::floor(camPos.x / chunkSize));
    const auto camChunkZ = static_cast<int>(std::floor(camPos.z / chunkSize));
    ChunkCoord currentCamCoord{camChunkX, camChunkZ};

    adjustLoadedChunks(currentCamCoord);
    reloadCurrentlyRelevantChunkGroup(currentCamCoord);
    runUpdatePerChunk();
}

void World::performFrustumCulling(const Frustum& frustum) {
    for (auto& [_, chunk] : loadedChunks) {
        chunk->performFrustumCulling(frustum);
    }
}

void World::renderByType(Shader& shader, CubeType type) {
    shader.use();
    for (auto& [_, chunk] : loadedChunks) {
        chunk->renderByType(shader, type);
    }
}