#include "World.hpp"

World::World() {
    for (int x{0}; x < worldSize; x++) {
        chunks.emplace_back(std::vector<std::unique_ptr<Chunk>>{});
        for (int z{0}; z < worldSize; z++) {
            chunks[x].emplace_back(std::make_unique<Chunk>(x, z));
        }
    }
}

void World::render(Shader& shader) {
    for (auto& x : chunks) {
        for (auto& z : x) {
            printf("render chunk\n");
            z->render(shader);
        }
    }
}

World::~World() {}