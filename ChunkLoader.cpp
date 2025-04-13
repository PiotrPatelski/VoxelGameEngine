#include "ChunkLoader.hpp"
#include "Chunk.hpp"
#include "VertexData.hpp"
#include <chrono>
#include <future>

ChunkLoader::ChunkLoader(int renderingDistance, int sizeOfChunk)
    : renderDistance(renderingDistance), chunkSize(sizeOfChunk) {
    setupVertexBuffers();
}

void ChunkLoader::setupVertexBuffers() {
    glGenBuffers(1, &vertexBufferObjects);
    glGenBuffers(1, &cubeElementBufferObjects);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeElementBufferObjects);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenBuffers(1, &waterElementBufferObjects);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterElementBufferObjects);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 waterIndices.size() * sizeof(unsigned int),
                 waterIndices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

std::unique_ptr<Chunk> ChunkLoader::createChunk(int x, int z) {
    return std::make_unique<Chunk>(chunkSize, x, z, vertexBufferObjects,
                                   cubeElementBufferObjects,
                                   waterElementBufferObjects);
}

std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>>
ChunkLoader::generateMissingChunks(
    int camChunkX, int camChunkZ,
    const std::unordered_set<ChunkCoord>& existingKeys) {
    std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>> newChunks;
    for (int x = camChunkX - renderDistance; x <= camChunkX + renderDistance;
         ++x) {
        for (int z = camChunkZ - renderDistance;
             z <= camChunkZ + renderDistance; ++z) {
            ChunkCoord coord{x, z};
            if (existingKeys.find(coord) == existingKeys.end()) {
                newChunks[coord] = createChunk(x, z);
            }
        }
    }
    return newChunks;
}

void ChunkLoader::launchTask(
    int camChunkX, int camChunkZ,
    const std::unordered_set<ChunkCoord>& existingKeys) {
    future = std::async(std::launch::async, &ChunkLoader::generateMissingChunks,
                        this, camChunkX, camChunkZ, existingKeys);
    isRunning = true;
}

bool ChunkLoader::isTaskRunning() const { return isRunning; }

bool ChunkLoader::isFinished() const {
    if (not isRunning) return false;
    return future.wait_for(std::chrono::milliseconds(0)) ==
           std::future_status::ready;
}

std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>>
ChunkLoader::retrieveNewChunks() {
    isRunning = false;
    return future.get();
}

ChunkLoader::~ChunkLoader() {
    glDeleteBuffers(1, &vertexBufferObjects);
    glDeleteBuffers(1, &cubeElementBufferObjects);
    glDeleteBuffers(1, &waterElementBufferObjects);
}