#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <glm/gtc/matrix_transform.hpp>
#include "Cube.hpp"
#include "Shader.hpp"
#include "Frustum.hpp"

struct IVec3Hash {
    std::size_t operator()(const glm::ivec3& v) const {
        std::size_t h1 = std::hash<int>()(v.x);
        std::size_t h2 = std::hash<int>()(v.y);
        std::size_t h3 = std::hash<int>()(v.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

class Chunk {
   public:
    Chunk(int size, int worldXindex, int worldZindex,
          unsigned vertexBufferObjects, unsigned elementBufferObjects,
          unsigned waterElementBufferObjects);
    ~Chunk();
    Chunk(const Chunk&) = delete;
    Chunk(Chunk&&) = delete;
    Chunk& operator=(const Chunk&) = delete;
    Chunk& operator=(Chunk&&) = delete;
    inline bool isCubeInGrid(const glm::vec3& position) const {
        return voxelGrid[position.x][position.z][position.y];
    }

    bool addCube(const glm::ivec3& localPos);
    bool removeCube(const glm::ivec3& localPos);

    void renderByType(Shader& shader, CubeType type);
    void performFrustumCulling(const Frustum& frustum);

   private:
    bool isPositionWithinBounds(const glm::ivec3& pos) const;
    void setupVAO(unsigned int sharedVBO, unsigned int sharedEBO);
    void generateInstanceBuffersForCubeTypes();
    void bindInstanceAttributesForType(CubeType cubeType);
    void rebuildCubesFromGrid();
    void rebuildVisibleInstances(const Frustum& frustum);
    void updateInstanceData();
    void uploadInstanceBuffer();
    void clearInstanceBuffer();
    void createCube(const glm::vec3& pos, CubeType type);
    void drawElements(CubeType type, unsigned int amount);
    void applyInitialTreeGrid();
    void reApplyExistingTrees(float initialCubeX, float initialCubeZ);
    void generateTrees(float initialCubeX, float initialCubeZ);
    void placeTreeAt(int x, int y, int z);
    std::vector<std::vector<std::vector<bool>>> generateInitialVoxelGrid();
    std::pair<glm::vec3, glm::vec3> computeChunkAABB() const;
    int findHighestCubeYval(int x, int z) const;

    int size{0};
    int chunkWorldXPosition{0};
    int chunkWorldZPosition{0};
    int waterHeight{0};
    std::vector<std::vector<std::vector<bool>>> voxelGrid{};
    std::vector<std::unique_ptr<Cube>> cubes{};
    std::unordered_set<glm::ivec3, IVec3Hash> treeTrunkPositions;
    std::unordered_map<CubeType, std::vector<glm::mat4>>
        instanceModelMatrices{};
    std::unordered_map<CubeType, unsigned int> instanceVBOs{};
    bool modified{true};
    unsigned int vao{};
    unsigned int regularCubeEBO{};
    unsigned int waterEBO{};
};