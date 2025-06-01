#include "World.hpp"
#include "Raycaster.hpp"
#include "VertexData.hpp"
#include "ChunkCoord.hpp"
#include "NeighborCubesGatherer.hpp"
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
        Raycaster{camera, chunkSize}.raycastDDA(loadedChunks, maxDistance);
    if (not hitOpt.has_value()) {
        return false;
    }

    const auto hit = hitOpt.value();
    const auto newCubePos = hit.position + hit.normal;
    const auto chunkX = floorDivide(newCubePos.x, chunkSize);
    const auto chunkZ = floorDivide(newCubePos.z, chunkSize);
    auto* chunk = getChunk({chunkX, chunkZ});
    if (not chunk) {
        return false;
    }

    const auto localX = negativeSafeModulo(newCubePos.x, chunkSize);
    const auto localZ = negativeSafeModulo(newCubePos.z, chunkSize);
    const auto localY = newCubePos.y;

    bool added = chunk->addCube({localX, localY, localZ}, type);
    if (added and type == CubeType::TORCH) {
        notifyNeighborChunks({chunkX, chunkZ});
    }
    return added;
}

bool World::removeCubeFromRaycast(const Camera& camera, float maxDistance) {
    auto hitOpt =
        Raycaster{camera, chunkSize}.raycastDDA(loadedChunks, maxDistance);
    if (not hitOpt.has_value()) {
        return false;
    }

    const auto hit = hitOpt.value();
    const auto worldCubePos = hit.position;
    const auto chunkX = floorDivide(worldCubePos.x, chunkSize);
    const auto chunkZ = floorDivide(worldCubePos.z, chunkSize);
    auto* chunk = getChunk({chunkX, chunkZ});
    if (not chunk) {
        return false;
    }

    const auto localX = negativeSafeModulo(worldCubePos.x, chunkSize);
    const auto localZ = negativeSafeModulo(worldCubePos.z, chunkSize);
    const auto localY = worldCubePos.y;

    if (chunk->getCubeType({localX, localY, localZ}) == CubeType::TORCH) {
        notifyNeighborChunks({chunkX, chunkZ});
    }
    return chunk->removeCube({localX, localY, localZ});
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

std::unordered_set<ChunkCoord, PositionXYHash> World::getLoadedChunkKeys() {
    std::unordered_set<ChunkCoord, PositionXYHash> keys;
    std::lock_guard<std::mutex> lock(loadedChunksMutex);
    for (const auto& [chunkCoord, _] : loadedChunks) {
        keys.insert(chunkCoord);
    }
    return keys;
}

void World::mergeNewChunks(Coord::CpuChunksMap& newChunks) {
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
    auto savedChunk = savedChunks.begin();
    while (savedChunk != savedChunks.end()) {
        const auto chunkCoord = savedChunk->first;
        if (isChunkWithinWindow(chunkCoord, window)) {
            restoreSavedChunk(chunkCoord, savedChunk);
        } else {
            ++savedChunk;
        }
    }
}

void World::restoreSavedChunk(const ChunkCoord& coord,
                              Coord::CpuChunksMap::iterator& savedChunk) {
    auto restoredRenderable = savedChunk->second->toRenderable(
        chunkLoader->getSharedVBO(), chunkLoader->getSharedEBO(),
        chunkLoader->getSharedWaterEBO());

    loadedChunks.emplace(coord, std::move(restoredRenderable));
    chunkUpdaters[coord] =
        std::make_unique<ChunkUpdater>(loadedChunks[coord].get());
    savedChunk = savedChunks.erase(savedChunk);
}

void World::evictOutOfRangeChunks(const ChunkWindow& window) {
    auto loadedChunk = loadedChunks.begin();
    while (loadedChunk != loadedChunks.end()) {
        const auto chunkCoord = loadedChunk->first;
        if (shouldEvictLoadedChunk(chunkCoord, window)) {
            evictLoadedChunk(chunkCoord, loadedChunk);
        } else {
            ++loadedChunk;
        }
    }
}

bool World::shouldEvictLoadedChunk(const ChunkCoord& coord,
                                   const ChunkWindow& window) const {
    bool outsideX = (coord.x < window.minX || coord.x > window.maxX);
    bool outsideZ = (coord.z < window.minZ || coord.z > window.maxZ);
    const bool isWithinWindow{!(outsideX || outsideZ)};
    if (isWithinWindow) {
        return false;
    }
    auto updaterIt = chunkUpdaters.find(coord);
    if (updaterIt != chunkUpdaters.end() &&
        updaterIt->second->isUpdateRunning()) {
        return false;
    }
    return true;
}

void World::evictLoadedChunk(
    const ChunkCoord& coord,
    Coord::RenderableChunksMap::iterator& loadedChunk) {
    auto cpuChunk = loadedChunk->second->toCpuChunk();
    savedChunks[coord] = std::move(cpuChunk);

    auto chunkUpdater = chunkUpdaters.find(coord);
    if (chunkUpdater != chunkUpdaters.end()) {
        chunkUpdaters.erase(chunkUpdater);
    }
    loadedChunk = loadedChunks.erase(loadedChunk);
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
    injectNeighborsToModifiedChunks();
    runUpdatePerChunk();
}

void World::injectNeighborsToModifiedChunks() {
    for (auto& [coord, chunk] : loadedChunks) {
        if (chunk->isModified()) {
            auto neighborData =
                NeighborGatherer{chunkSize}.gatherNeighborsForCoord(
                    coord, [this](const ChunkCoord& chunkCoord) {
                        return this->getChunk(chunkCoord);
                    });
            chunk->setNeighborsSurroundingCubes(std::move(neighborData));
        }
    }
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

void World::notifyNeighborChunks(const ChunkCoord& centerCoord) {
    for (int offsetX = -1; offsetX <= 1; ++offsetX) {
        for (int offsetZ = -1; offsetZ <= 1; ++offsetZ) {
            if (offsetX == 0 && offsetZ == 0) {
                continue;
            }
            ChunkCoord neighborCoord{centerCoord.x + offsetX,
                                     centerCoord.z + offsetZ};
            if (auto* neighbor = getChunk(neighborCoord)) {
                neighbor->markModified();
            }
        }
    }
}

World::~World() { std::cout << "World::Shutdown!" << std::endl; }