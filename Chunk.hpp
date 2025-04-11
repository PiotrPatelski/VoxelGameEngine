#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <utility>
#include <glm/gtc/matrix_transform.hpp>
#include "Cube.hpp"
#include "Shader.hpp"
#include "Frustum.hpp"
class Chunk {
   public:
    Chunk(int size, int worldXindex, int worldZindex,
          unsigned vertexBufferObjects, unsigned elementBufferObjects);
    ~Chunk();
    Chunk(const Chunk&) = delete;
    Chunk(Chunk&&) = delete;
    Chunk& operator=(const Chunk&) = delete;
    Chunk& operator=(Chunk&&) = delete;
    inline bool isCubeInGrid(const glm::vec3& position) const {
        return cubeGrid[position.x][position.z][position.y];
    }

    bool addCube(const glm::ivec3& localPos);
    bool removeCube(const glm::ivec3& localPos);

    void renderByType(Shader& shader, CubeType type, size_t indicesSize);
    void performFrustumCulling(const Frustum& frustum);

   private:
    bool isPositionWithinBounds(const glm::ivec3& pos) const;
    void setupVAO(unsigned int sharedVBO, unsigned int sharedEBO);
    void generateInstanceBuffersForCubeTypes();
    void rebuildCubesFromGrid();
    void rebuildVisibleInstances(const Frustum& frustum);
    void updateInstanceData();
    void uploadInstanceBuffer();
    void clearInstanceBuffer();
    void createCube(const glm::vec3& pos, CubeType type);

    std::vector<std::vector<std::vector<bool>>> generateInitialCubeGrid();
    std::pair<glm::vec3, glm::vec3> computeChunkAABB() const;

    int size{0};
    int chunkWorldXPosition{0};
    int chunkWorldZPosition{0};
    int waterHeight{0};
    std::vector<std::vector<std::vector<bool>>> cubeGrid{};
    std::vector<std::unique_ptr<Cube>> cubes{};
    std::unordered_map<CubeType, std::vector<glm::mat4>> instanceMatrices{};
    std::unordered_map<CubeType, unsigned int> instanceBuffers{};
    bool modified{true};
    unsigned int vao{};
};