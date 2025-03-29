#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <utility>
#include <glm/gtc/matrix_transform.hpp>
#include "Cube.hpp"
#include "Shader.hpp"
#include "Frustum.hpp"

struct Vertex {
    glm::vec3 position{};
    glm::vec2 texCoord{};
    glm::vec3 normal{};
};

enum class Voxel { NONE, SAND, DIRT, GRASS };

class Chunk {
   public:
    Chunk(int size, int worldXindex, int worldZindex,
          unsigned vertexBufferObjects, unsigned elementBufferObjects);
    ~Chunk();
    inline std::pair<int, int> getWorldPosition() const {
        return {chunkWorldXPosition, chunkWorldZPosition};
    }
    inline bool isCubeInGrid(const glm::vec3& position) const {
        return cubeGrid[position.x][position.z][position.y];
    }
    inline const std::vector<std::vector<std::vector<bool>>>& getCubeGrid()
        const {
        return cubeGrid;
    }
    inline const std::vector<std::vector<std::vector<Voxel>>>& getVoxelGrid()
        const {
        return voxelGrid;
    }
    bool addCube(const glm::ivec3& localPos, CubeType type);
    bool removeCube(const glm::ivec3& localPos);

    void updateInstanceBuffer();
    void renderByType(Shader& shader, CubeType type);
    void performFrustumCulling(const Frustum& frustum);

    static inline constexpr std::vector<Vertex> getVertices() {
        return vertices;
    }
    static inline constexpr std::vector<unsigned int> getIndices() {
        return indices;
    }

   private:
    void setupVAO(unsigned int sharedVBO, unsigned int sharedEBO);
    void generateInstanceBuffersForCubeTypes();
    std::vector<std::vector<std::vector<bool>>> generateInitialCubeGrid();
    void rebuildCubesFromGrid();
    void uploadInstanceData();

    void computeChunkAABB(glm::vec3& outMin, glm::vec3& outMax) const {
        // We assume the chunk’s origin is at (chunkWorldXPosition * size, 0,
        // chunkWorldZPosition * size)
        outMin = glm::vec3(chunkWorldXPosition * size, 0.0f,
                           chunkWorldZPosition * size);
        outMax = outMin + glm::vec3(size);
    }

    int size{0};
    int chunkWorldXPosition{0};
    int chunkWorldZPosition{0};
    static std::vector<Vertex> vertices;
    static std::vector<unsigned int> indices;
    std::vector<std::vector<std::vector<bool>>> cubeGrid;
    std::vector<std::vector<std::vector<Voxel>>> voxelGrid;
    std::vector<std::unique_ptr<Cube>> cubes{};
    std::unordered_map<CubeType, std::vector<glm::mat4>> instanceMatrices;
    std::unordered_map<CubeType, unsigned int> instanceBuffers;

    unsigned int vao{};
};