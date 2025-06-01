#include "ChunkUpdater.hpp"

ChunkUpdater::ChunkUpdater(RenderableChunk* chunkToUpdate)
    : chunk(chunkToUpdate), isUpdating(false) {}

ChunkUpdater::~ChunkUpdater() {}

void ChunkUpdater::launchUpdate() {
    if (not isUpdating) {
        isUpdating = true;
        auto* target = chunk;
        updateResult = std::async(std::launch::async, [target]() {
            if (target) {
                return target->computeCubeData();
            }
            return CubeData{};
        });
    }
}

void ChunkUpdater::checkAndApplyUpdate() {
    if (isUpdating and chunk) {
        if (updateResult.wait_for(std::chrono::milliseconds(0)) ==
            std::future_status::ready) {
            chunk->applyCubeData(updateResult.get());
            chunk->clearNeighborsSurroundingCubes();
            isUpdating = false;
        }
    }
}