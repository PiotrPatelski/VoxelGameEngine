#pragma once
#include <future>
#include <chrono>
#include "RenderableChunk.hpp"

class ChunkUpdater {
   public:
    explicit ChunkUpdater(RenderableChunk* chunk);
    ~ChunkUpdater();
    ChunkUpdater(const ChunkUpdater&) = delete;
    ChunkUpdater(ChunkUpdater&&) = delete;
    ChunkUpdater& operator=(const ChunkUpdater&) = delete;
    ChunkUpdater& operator=(ChunkUpdater&&) = delete;
    inline bool isUpdateRunning() const { return isUpdating; }

    void launchUpdate();
    void checkAndApplyUpdate();

   private:
    RenderableChunk* chunk{};
    std::future<CubeData> updateResult{};
    bool isUpdating{false};
};