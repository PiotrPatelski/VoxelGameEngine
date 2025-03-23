#pragma once
#include <vector>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include "Cube.hpp"
#include "Shader.hpp"
#include "Frustum.hpp"

class Chunk {
   public:
    Chunk(int worldXindex, int worldZindex, unsigned vertexBufferObjects,
          unsigned elementBufferObjects);
    ~Chunk();
    static inline const std::vector<Vertex> getVertices() { return vertices; }
    static inline const std::vector<unsigned int> getIndices() {
        return indices;
    }
    bool isSurroundedCube(const Cube& cube) const;
    void render(Shader& shader);
    void applyCubeModels();

   private:
    auto positionToIndex(const glm::vec3& position) const;
    bool isCubeOnBorder(const glm::vec3& position) const;
    int chunkWorldXPosition{0};
    int chunkWorldZPosition{0};
    static std::vector<Vertex> vertices;
    static std::vector<unsigned int> indices;
    std::vector<std::unique_ptr<Cube>> cubes{};
    std::vector<glm::mat4> modelMatrices{};
    unsigned int buffer{};
    unsigned int vao{};
};