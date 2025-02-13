#pragma once
#include <vector>
#include "Chunk.hpp"
#include "Shader.hpp"

class World {
   public:
    World();
    ~World();

    void render(Shader& shader);

   private:
    static constexpr int worldSize{2};
    std::vector<std::vector<std::unique_ptr<Chunk>>> chunks{};
};