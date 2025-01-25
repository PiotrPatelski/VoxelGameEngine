#pragma once
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "Cube.hpp"
#include "Shader.hpp"

class Chunk {
   public:
    Chunk();
    ~Chunk();
    static inline const std::vector<float> getVertices() { return vertices; }
    inline std::vector<std::vector<std::vector<Cube>>> getCubes() const {
        return cubes;
    }
    void render(Shader& shader);

   private:
    static std::vector<float> vertices;
    std::vector<std::vector<std::vector<Cube>>> cubes{};
};