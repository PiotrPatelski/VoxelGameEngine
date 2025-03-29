#pragma once
#include <vector>
#include <optional>
#include "Chunk.hpp"
#include "Shader.hpp"
#include "Frustum.hpp"

struct HitResult {
    glm::ivec3 position;
    int chunkX;
    int chunkZ;
    bool hit = false;
};

class World {
   public:
    World();
    ~World();
    std::optional<HitResult> raycast(const glm::vec3& origin,
                                     const glm::vec3& direction,
                                     float maxDistance = 5.0f) const;
    void performFrustumCulling(const class Frustum& frustum);
    void renderByType(Shader& shader, CubeType type);
    Chunk* getChunk(int chunkX, int chunkZ);

   private:
    std::vector<std::vector<std::unique_ptr<Chunk>>> chunks{};
    static constexpr int worldSize{8};
};