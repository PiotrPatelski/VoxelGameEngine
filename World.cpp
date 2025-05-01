#include "World.hpp"
#include "Raycaster.hpp"
#include "VertexData.hpp"
#include <cstdio>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

namespace {
ChunkWindow computeChunkWindow(const ChunkCoord& cameraChunk,
                               int renderDistance) {
    return {cameraChunk.x - renderDistance, cameraChunk.x + renderDistance,
            cameraChunk.z - renderDistance, cameraChunk.z + renderDistance};
}
} // namespace

void World::loadInitialChunks() {
    std::lock_guard<std::mutex> lock(loadedChunksMutex);
    for (int x = -renderDistance; x <= renderDistance; ++x) {
        for (int z = -renderDistance; z <= renderDistance; ++z) {
            ChunkCoord coord{x, z};
            auto chunk = chunkLoader->createChunk(x, z);
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
        auto chunk = getChunk({chunkX, chunkZ});
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
        auto chunk = getChunk({chunkX, chunkZ});
        if (chunk) {
            const auto localX = negativeSafeModulo(worldCubePos.x, chunkSize);
            const auto localZ = negativeSafeModulo(worldCubePos.z, chunkSize);
            const auto localY = worldCubePos.y;
            return chunk->removeCube(glm::ivec3(localX, localY, localZ));
        }
    }
    return false;
}

RenderableChunk* World::getChunk(const ChunkCoord& coord) {
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
    std::unordered_map<ChunkCoord, std::unique_ptr<CpuChunk>>& newChunks) {
    std::lock_guard<std::mutex> lock(loadedChunksMutex);
    for (auto& [coord, newCpuChunk] : newChunks) {
        auto [it, inserted] = loadedChunks.try_emplace(coord, nullptr);
        if (inserted) {
            auto& newGpuChunk = it->second;
            newGpuChunk = newCpuChunk->toRenderable(
                chunkLoader->getSharedVBO(), chunkLoader->getSharedEBO(),
                chunkLoader->getSharedWaterEBO());
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

void World::restoreSavedChunks(const ChunkWindow& window) {
    for (auto currentSavedChunk = savedChunks.begin();
         currentSavedChunk != savedChunks.end();) {
        const auto chunkCoord = currentSavedChunk->first;
        auto& savedCpuChunk = currentSavedChunk->second;
        bool withinX =
            (chunkCoord.x >= window.minX and chunkCoord.x <= window.maxX);
        bool withinZ =
            (chunkCoord.z >= window.minZ and chunkCoord.z <= window.maxZ);
        if (withinX and withinZ) {
            auto restoredChunk = savedCpuChunk->toRenderable(
                chunkLoader->getSharedVBO(), chunkLoader->getSharedEBO(),
                chunkLoader->getSharedWaterEBO());
            loadedChunks.emplace(chunkCoord, std::move(restoredChunk));
            chunkUpdaters[chunkCoord] =
                std::make_unique<ChunkUpdater>(loadedChunks[chunkCoord].get());
            currentSavedChunk = savedChunks.erase(currentSavedChunk);
        } else {
            ++currentSavedChunk;
        }
    }
}

void World::evictOutOfRangeChunks(const ChunkWindow& window) {
    for (auto currentLoadedChunk = loadedChunks.begin();
         currentLoadedChunk != loadedChunks.end();) {
        const auto& [chunkCoord, renderableChunk] = *currentLoadedChunk;
        bool outsideX =
            (chunkCoord.x < window.minX or chunkCoord.x > window.maxX);
        bool outsideZ =
            (chunkCoord.z < window.minZ or chunkCoord.z > window.maxZ);
        if (outsideX or outsideZ) {
            auto updaterIt = chunkUpdaters.find(chunkCoord);
            if (updaterIt != chunkUpdaters.end() &&
                updaterIt->second->isUpdateRunning()) {
                ++currentLoadedChunk;
                continue;
            }
            auto cpuChunk = renderableChunk->toCpuChunk();
            savedChunks[chunkCoord] = std::move(cpuChunk);
            if (updaterIt != chunkUpdaters.end()) {
                chunkUpdaters.erase(updaterIt);
            }
            currentLoadedChunk = loadedChunks.erase(currentLoadedChunk);
        } else {
            ++currentLoadedChunk;
        }
    }
}

void World::adjustLoadedChunks(const ChunkCoord& currentCamCoord) {
    const auto window = computeChunkWindow(currentCamCoord, renderDistance);

    std::lock_guard<std::mutex> lock(loadedChunksMutex);
    restoreSavedChunks(window);
    evictOutOfRangeChunks(window);
}

void World::runUpdatePerChunk() {
    for (auto& [coord, updater] : chunkUpdaters) {
        const auto chunk = getChunk(coord);
        if (chunk and chunk->isModified() and !updater->isUpdateRunning()) {
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
        chunk->renderByType(type);
    }
}