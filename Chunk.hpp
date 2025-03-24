#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>
#include "Cube.hpp"
#include "Shader.hpp"
#include "Frustum.hpp"

struct Vertex {
    glm::vec3 position{};
    glm::vec3 color{};
    glm::vec2 texCoord{};
    glm::vec3 normal{};
};

class Chunk {
   public:
    Chunk(int worldXindex, int worldZindex, unsigned vertexBufferObjects,
          unsigned elementBufferObjects);
    ~Chunk();
    static inline constexpr std::vector<Vertex> getVertices() {
        return vertices;
    }
    static inline constexpr std::vector<unsigned int> getIndices() {
        return indices;
    }
    void updateInstanceBuffer();
    void render(Shader& shader);
    void renderByType(Shader& shader, CubeType type);
    void performFrustumCulling(const Frustum& frustum);

   private:
    void setupVAO(unsigned int sharedVBO, unsigned int sharedEBO);
    void generateInstanceBuffersForCubeTypes();
    void generateCubeData();
    std::vector<std::vector<std::vector<bool>>> generateCubePresence();
    void generateVisibleCubes(
        const std::vector<std::vector<std::vector<bool>>>& cubePresent,
        float initialCubeX, float initialCubeZ);
    void uploadInstanceData();

    int chunkWorldXPosition{0};
    int chunkWorldZPosition{0};
    static std::vector<Vertex> vertices;
    static std::vector<unsigned int> indices;
    std::vector<std::unique_ptr<Cube>> cubes{};
    std::unordered_map<CubeType, std::vector<glm::mat4>> instanceMatrices;
    std::unordered_map<CubeType, unsigned int> instanceBuffers;

    unsigned int vao{};
    unsigned int bufferSand{};
    unsigned int bufferDirt{};
    unsigned int bufferGrass{};
};