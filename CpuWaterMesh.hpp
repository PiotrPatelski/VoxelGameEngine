#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include "VertexData.hpp"
#include "Cube.hpp"

class CpuWaterMesh {
   public:
    CpuWaterMesh(const std::vector<Vertex>& vertexData,
                 const std::vector<unsigned int>& indexData, CubeType type,
                 const glm::vec3& centerPos)
        : vertices(vertexData),
          indices(indexData),
          waterType(type),
          center(centerPos) {}

    const std::vector<Vertex>& getVertices() const { return vertices; }
    const std::vector<unsigned int>& getIndices() const { return indices; }
    CubeType getWaterType() const { return waterType; }
    glm::vec3 getCenter() const { return center; }
    bool isEmpty() const { return indices.empty(); }

   private:
    std::vector<Vertex> vertices{};
    std::vector<unsigned int> indices{};
    CubeType waterType{CubeType::WATER_SOURCE};
    glm::vec3 center{};
};
