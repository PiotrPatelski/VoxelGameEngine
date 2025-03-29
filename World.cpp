#include "World.hpp"

static constexpr int chunkSize{64};
// BUFFER DEFAULTS
unsigned int vertexBufferObjects,
    elementBufferObjects; // must be overwritten before use
// unsigned int lightSourceVAO;
const unsigned int amountOfVBOBuffers{1};
const unsigned int amountOfEBOBuffers{1};

void setupVertexBufferData() {
    glGenBuffers(amountOfVBOBuffers, &vertexBufferObjects);
    glGenBuffers(amountOfEBOBuffers, &elementBufferObjects);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects);
    glBufferData(GL_ARRAY_BUFFER, Chunk::getVertices().size() * sizeof(Vertex),
                 Chunk::getVertices().data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObjects);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 Chunk::getIndices().size() * sizeof(unsigned int),
                 Chunk::getIndices().data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

World::World() {
    printf("World::Init!\n");
    setupVertexBufferData();

    for (int x{0}; x < worldSize; x++) {
        chunks.emplace_back(std::vector<std::unique_ptr<Chunk>>{});
        for (int z{0}; z < worldSize; z++) {
            chunks[x].emplace_back(std::make_unique<Chunk>(
                chunkSize, x, z, vertexBufferObjects, elementBufferObjects));
        }
    }
}

void World::performFrustumCulling(const Frustum &frustum) {
    for (auto &chunkRow : chunks) {
        for (auto &chunk : chunkRow) {
            chunk->performFrustumCulling(frustum);
        }
    }
}

void World::renderByType(Shader &shader, CubeType type) {
    shader.use();
    for (auto &chunkRow : chunks) {
        for (auto &chunk : chunkRow) {
            chunk->renderByType(shader, type);
        }
    }
}

std::optional<HitResult> World::raycast(const glm::vec3 &origin,
                                        const glm::vec3 &direction,
                                        float maxDistance) const {
    glm::vec3 rayPos = origin;
    glm::vec3 rayDir = glm::normalize(direction);
    float t = 0.0f;
    const float delta = 0.1f;
    while (t < maxDistance) {
        glm::ivec3 blockPos = glm::floor(rayPos);
        int chunkX = blockPos.x / chunkSize;
        int chunkZ = blockPos.z / chunkSize;
        if (chunkX < 0 || chunkX >= worldSize || chunkZ < 0 ||
            chunkZ >= worldSize) {
            t += delta;
            rayPos = origin + t * rayDir;
            continue;
        }
        const Chunk *chunk = chunks[chunkX][chunkZ].get();
        const auto &grid = chunk->getCubeGrid();
        int localX = blockPos.x % chunkSize;
        int localZ = blockPos.z % chunkSize;
        int localY = blockPos.y;
        if (localX < 0) localX += chunkSize;
        if (localZ < 0) localZ += chunkSize;
        if (localY >= 0 && localY < chunkSize && grid[localX][localZ][localY]) {
            HitResult result;
            result.position = blockPos;
            result.chunkX = chunkX;
            result.chunkZ = chunkZ;
            result.hit = true;
            return result;
        }
        t += delta;
        rayPos = origin + t * rayDir;
    }
    return std::nullopt;
}

Chunk *World::getChunk(int chunkX, int chunkZ) {
    if (chunkX < 0 || chunkX >= worldSize || chunkZ < 0 || chunkZ >= worldSize)
        return nullptr;
    return chunks[chunkX][chunkZ].get();
}

World::~World() {
    glDeleteBuffers(amountOfVBOBuffers, &vertexBufferObjects);
    glDeleteBuffers(amountOfEBOBuffers, &elementBufferObjects);
}