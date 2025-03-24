#include "World.hpp"

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
                x, z, vertexBufferObjects, elementBufferObjects));
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

void World::render(Shader &shader) {
    shader.use();
    for (auto &chunkRow : chunks) {
        for (auto &chunk : chunkRow) {
            chunk->render(shader);
        }
    }
}

World::~World() {
    glDeleteBuffers(amountOfVBOBuffers, &vertexBufferObjects);
    glDeleteBuffers(amountOfEBOBuffers, &elementBufferObjects);
}