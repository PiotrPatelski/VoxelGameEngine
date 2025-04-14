#pragma once
#include <future>
#include <chrono>
#include "Chunk.hpp"

class ChunkUpdater {
   public:
    explicit ChunkUpdater(Chunk* chunk);
    ~ChunkUpdater();
    ChunkUpdater(const ChunkUpdater&) = delete;
    ChunkUpdater(ChunkUpdater&&) = delete;
    ChunkUpdater& operator=(const ChunkUpdater&) = delete;
    ChunkUpdater& operator=(ChunkUpdater&&) = delete;
    inline bool isUpdateRunning() const { return isUpdating; }

    void launchUpdate();
    void checkAndApplyUpdate();

   private:
    Chunk* chunk{};
    std::future<CubeData> updateResult{};
    bool isUpdating{false};
};