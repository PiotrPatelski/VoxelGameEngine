#pragma once
#include <vector>
#include "Chunk.hpp"
#include "Shader.hpp"
#include "Frustum.hpp"

class World {
   public:
    World();
    ~World();
    void performFrustumCulling(const class Frustum& frustum);
    void render(Shader& shader);
    std::vector<std::vector<std::unique_ptr<Chunk>>> chunks{};

   private:
    static constexpr int worldSize{4};
};