#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <future>
#include <glm/gtc/matrix_transform.hpp>
#include "Cube.hpp"
#include "Shader.hpp"
#include "Frustum.hpp"
#include "TreeManager.hpp"
#include "GridGenerator.hpp"

struct CubeData {
    std::vector<std::unique_ptr<Cube>> cubes;
    std::unordered_map<CubeType, std::vector<glm::mat4>> instanceModelMatrices;
};

class Chunk {
   public:
    Chunk(int size, int worldXindex, int worldZindex,
          unsigned vertexBufferObjects, unsigned elementBufferObjects,
          unsigned waterElementBufferObjects);
    Chunk(int size, int worldXindex, int worldZindex, bool cpuOnly);
    ~Chunk();
    Chunk(const Chunk&) = delete;
    Chunk(Chunk&&) = delete;
    Chunk& operator=(const Chunk&) = delete;
    Chunk& operator=(Chunk&&) = delete;
    inline bool isCubeInGrid(const glm::vec3& position) const {
        return voxelGrid[position.x][position.z][position.y] != CubeType::NONE;
    }
    inline bool isModified() const { return modified; }
    bool addCube(const glm::ivec3& localPos, CubeType type);
    bool removeCube(const glm::ivec3& localPos);

    void renderByType(Shader& shader, CubeType type);
    void performFrustumCulling(const Frustum& frustum);

    CubeData computeCubeData();
    void applyCubeData(CubeData&& data);

    void initializeGL(unsigned int sharedVBO, unsigned int sharedEBO,
                      unsigned int sharedWaterEBO);

   private:
    using CubeCreator = std::function<void(const glm::vec3&, CubeType)>;
    void setupVAO(unsigned int sharedVBO, unsigned int sharedEBO);
    void generateInstanceBuffersForCubeTypes();
    void bindInstanceAttributesForType(CubeType cubeType);
    void processVoxelGrid(float initialCubeX, float initialCubeZ,
                          const CubeCreator& cubeAdder);
    void rebuildCubesFromGrid();
    void regenerateChunk(const CubeCreator& applier);
    void uploadInstanceBuffer();
    void clearInstanceBuffer();
    void createCube(const glm::vec3& worldCubePos, CubeType type);
    void drawElements(CubeType type, unsigned int amount);
    GridGenerator::VoxelGrid generateInitialVoxelGrid();
    std::pair<glm::vec3, glm::vec3> computeChunkAABB() const;

    int size{0};
    int chunkWorldXIndex{0};
    int chunkWorldZIndex{0};
    int waterHeight{14};
    TreeManager treeManager;
    GridGenerator::VoxelGrid voxelGrid{};
    std::vector<std::unique_ptr<Cube>> cubes{};
    std::unordered_map<CubeType, std::vector<glm::mat4>>
        instanceModelMatrices{};
    std::unordered_map<CubeType, unsigned int> instanceVBOs{};
    bool modified{true};
    unsigned int vao{};
    unsigned int regularCubeEBO{};
    unsigned int waterEBO{};

    bool isCulled{false};
    std::mutex voxelMutex;
};