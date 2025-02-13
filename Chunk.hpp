#pragma once
#include <vector>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include "Cube.hpp"
#include "Shader.hpp"

class Chunk {
   public:
    Chunk(int worldXindex, int worldZindex);
    ~Chunk();
    static inline const std::vector<float> getVertices() { return vertices; }
    bool isSurroundedCube(const Cube& cube) const;
    void render(Shader& shader);

   private:
    int chunkWorldXPosition{0};
    int chunkWorldZPosition{0};
    static std::vector<float> vertices;
    std::vector<std::vector<std::vector<std::unique_ptr<Cube>>>> cubes{};
};