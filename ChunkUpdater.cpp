#include "ChunkUpdater.hpp"

ChunkUpdater::ChunkUpdater(Chunk* chunkToUpdate)
    : chunk(chunkToUpdate), isUpdating(false) {}

ChunkUpdater::~ChunkUpdater() {}

void ChunkUpdater::launchUpdate() {
    if (not isUpdating) {
        isUpdating = true;
        updateResult = std::async(
            std::launch::async, [this]() { return chunk->computeCubeData(); });
    }
}

void ChunkUpdater::checkAndApplyUpdate() {
    if (isUpdating) {
        if (updateResult.wait_for(std::chrono::milliseconds(0)) ==
            std::future_status::ready) {
            CubeData data = updateResult.get();
            chunk->applyCubeData(std::move(data));
            isUpdating = false;
        }
    }
}