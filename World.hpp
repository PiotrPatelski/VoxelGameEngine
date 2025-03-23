#pragma once
#include <vector>
#include "Chunk.hpp"
#include "Shader.hpp"

class World {
   public:
    World();
    ~World();

    void render(Shader& shader);
    std::vector<std::vector<std::unique_ptr<Chunk>>> chunks{};

   private:
    static constexpr int worldSize{2};
};