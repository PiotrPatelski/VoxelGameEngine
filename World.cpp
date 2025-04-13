#include "World.hpp"
#include "Raycaster.hpp"
#include "VertexData.hpp"
#include <cstdio>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

namespace {
unsigned int vertexBufferObjects{}, elementBufferObjects{};
unsigned int waterElementBufferObjects{};
constexpr unsigned int amountOfVBOBuffers{1};
constexpr unsigned int amountOfEBOBuffers{1};

void setupVertexBufferData() {
    glGenBuffers(amountOfVBOBuffers, &vertexBufferObjects);
    glGenBuffers(amountOfEBOBuffers, &elementBufferObjects);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObjects);
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
} // namespace

World::World() {
    printf("World::Init!\n");
    setupVertexBufferData();

    // Initially load chunks around (0,0) using renderDistance.
    for (int x = -renderDistance; x <= renderDistance; x++) {
        for (int z = -renderDistance; z <= renderDistance; z++) {
            ChunkCoord coord{x, z};
            loadedChunks[coord] = std::make_unique<Chunk>(
                chunkSize, x, z, vertexBufferObjects, elementBufferObjects,
                waterElementBufferObjects);
        }
    }
    lastCameraChunk = {0, 0};
}

bool World::addCubeFromRaycast(const Camera& camera, float maxDistance) {
    auto hitOpt =
        Raycaster{camera, chunkSize}.raycast(loadedChunks, maxDistance);
    if (hitOpt.has_value()) {
        HitResult hit = hitOpt.value();
        // For this example, we add a cube above the hit block.
        glm::ivec3 worldBlockPos = hit.position;
        worldBlockPos.y += 1;
        // TODO REFACTOR DUPLICATE WITH THE ONE FROM removeCubeFromRaycast
        int chunkX = worldBlockPos.x / chunkSize;
        int chunkZ = worldBlockPos.z / chunkSize;
        int localX = worldBlockPos.x % chunkSize;
        int localZ = worldBlockPos.z % chunkSize;
        int localY = worldBlockPos.y;
        Chunk* chunk = getChunk({chunkX, chunkZ});
        if (chunk) {
            return chunk->addCube(glm::ivec3(localX, localY, localZ));
        }
    }
    return false;
}

bool World::removeCubeFromRaycast(const Camera& camera, float maxDistance) {
    auto hitOpt =
        Raycaster{camera, chunkSize}.raycast(loadedChunks, maxDistance);
    if (hitOpt.has_value()) {
        HitResult hit = hitOpt.value();
        glm::ivec3 worldBlockPos = hit.position;
        int chunkX = worldBlockPos.x / chunkSize;
        int chunkZ = worldBlockPos.z / chunkSize;
        int localX = worldBlockPos.x % chunkSize;
        int localZ = worldBlockPos.z % chunkSize;
        int localY = worldBlockPos.y;
        Chunk* chunk = getChunk({chunkX, chunkZ});
        if (chunk) {
            return chunk->removeCube(glm::ivec3(localX, localY, localZ));
        }
    }
    return false;
}

Chunk* World::getChunk(const ChunkCoord& coord) const {
    auto it = loadedChunks.find(coord);
    return (it != loadedChunks.end()) ? it->second.get() : nullptr;
}

void World::updateLoadedChunks(const glm::vec3& camPos) {
    int camChunkX = static_cast<int>(std::floor(camPos.x / chunkSize));
    int camChunkZ = static_cast<int>(std::floor(camPos.z / chunkSize));
    ChunkCoord camCoord{camChunkX, camChunkZ};
    if (camCoord == lastCameraChunk) return;
    lastCameraChunk = camCoord;

    std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>> newLoaded;
    for (int x = camChunkX - renderDistance; x <= camChunkX + renderDistance;
         x++) {
        for (int z = camChunkZ - renderDistance;
             z <= camChunkZ + renderDistance; z++) {
            ChunkCoord coord{x, z};
            if (loadedChunks.find(coord) != loadedChunks.end()) {
                newLoaded[coord] = std::move(loadedChunks[coord]);
            } else {
                newLoaded[coord] = std::make_unique<Chunk>(
                    chunkSize, x, z, vertexBufferObjects, elementBufferObjects,
                    waterElementBufferObjects);
            }
        }
    }
    loadedChunks = std::move(newLoaded);
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

World::~World() {
    glDeleteBuffers(amountOfVBOBuffers, &vertexBufferObjects);
    glDeleteBuffers(amountOfEBOBuffers, &elementBufferObjects);
    glDeleteBuffers(1, &waterElementBufferObjects);
}