#include "World.hpp"
#include "Raycaster.hpp"
#include "VertexData.hpp"
#include <cstdio>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

void World::loadInitialChunks() {
    std::lock_guard<std::mutex> lock(loadedChunksMutex);
    // Initially load chunks around (0,0) using renderDistance.
    for (int x = -renderDistance; x <= renderDistance; x++) {
        for (int z = -renderDistance; z <= renderDistance; z++) {
            ChunkCoord coord{x, z};
            loadedChunks[coord] = chunkLoader->createChunk(x, z);
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

bool World::addCubeFromRaycast(const Camera& camera, float maxDistance) {
    const auto hitOpt =
        Raycaster{camera, chunkSize}.raycast(loadedChunks, maxDistance);
    if (hitOpt.has_value()) {
        HitResult hit = hitOpt.value();
        // For this example, we add a cube above the hit block.
        glm::ivec3 worldBlockPos = hit.position;
        worldBlockPos.y += 1;
        int chunkX = floorDiv(worldBlockPos.x, chunkSize);
        int chunkZ = floorDiv(worldBlockPos.z, chunkSize);
        int localX = mod(worldBlockPos.x, chunkSize);
        int localZ = mod(worldBlockPos.z, chunkSize);
        const auto localY = worldBlockPos.y;
        Chunk* chunk = getChunk({chunkX, chunkZ});
        if (chunk) {
            return chunk->addCube(glm::ivec3(localX, localY, localZ));
        }
    }
    return false;
}

bool World::removeCubeFromRaycast(const Camera& camera, float maxDistance) {
    const auto hitOpt =
        Raycaster{camera, chunkSize}.raycast(loadedChunks, maxDistance);
    if (hitOpt.has_value()) {
        HitResult hit = hitOpt.value();
        glm::ivec3 worldBlockPos = hit.position;
        int chunkX = floorDiv(worldBlockPos.x, chunkSize);
        int chunkZ = floorDiv(worldBlockPos.z, chunkSize);
        int localX = mod(worldBlockPos.x, chunkSize);
        int localZ = mod(worldBlockPos.z, chunkSize);
        const auto localY = worldBlockPos.y;
        Chunk* chunk = getChunk({chunkX, chunkZ});
        if (chunk) {
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
    for (auto& [chunkCoord, chunk] : newChunks) {
        loadedChunks.try_emplace(chunkCoord, std::move(chunk));
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