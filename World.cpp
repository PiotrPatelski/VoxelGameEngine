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
    glm::vec3 rayDir = glm::normalize(direction);

    // Convert start position to integer voxel coordinates
    glm::ivec3 blockPos = glm::floor(origin);
    glm::ivec3 step = glm::sign(rayDir);

    // Precompute inverse direction for efficiency
    glm::vec3 invRayDir = 1.0f / rayDir;

    // Compute first intersection distances
    glm::vec3 tMax =
        (glm::vec3(blockPos) + glm::vec3(step) * 0.5f - origin) * invRayDir;
    glm::vec3 tDelta = glm::abs(invRayDir);

    float distanceTraveled = 0.0f;

    while (distanceTraveled < maxDistance) {
        // Compute chunk coordinates
        int chunkX = blockPos.x / chunkSize;
        int chunkZ = blockPos.z / chunkSize;

        // Check if out of world bounds early
        if (chunkX < 0 || chunkX >= worldSize || chunkZ < 0 ||
            chunkZ >= worldSize)
            return std::nullopt;

        // Retrieve chunk
        const Chunk *chunk = chunks[chunkX][chunkZ].get();
        const auto &grid = chunk->getCubeGrid();

        // Convert world-space to chunk-local coordinates (avoiding modulo)
        int localX =
            blockPos.x &
            (chunkSize -
             1); // Faster than `% chunkSize` if chunkSize is power of 2
        int localZ = blockPos.z & (chunkSize - 1);
        int localY = blockPos.y;

        // Check if voxel is solid
        if (localY >= 0 && localY < chunkSize && grid[localX][localZ][localY]) {
            return HitResult{blockPos, chunkX, chunkZ, true};
        }

        // Move to next voxel along the ray
        int axis = (tMax.x < tMax.y) ? ((tMax.x < tMax.z) ? 0 : 2)
                                     : ((tMax.y < tMax.z) ? 1 : 2);
        blockPos[axis] += step[axis];
        tMax[axis] += tDelta[axis];

        // Instead of glm::length(), track accumulated distance manually
        distanceTraveled += tDelta[axis];
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