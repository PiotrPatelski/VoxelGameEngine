#pragma once
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "VertexData.hpp"
#include <variant>
#include <optional>

std::vector<Vertex> createCubeVertexVector();
std::vector<EntityVertex> createEntityVertexVector(
    const std::vector<glm::vec2>& texCoords);

class VertexDataBuilder {
   public:
    VertexDataBuilder& withTexCoords(const std::vector<glm::vec2>& texCoords);
    VertexDataBuilder& withNormals(const std::vector<glm::vec3>& normals);

    std::vector<Vertex> createCubeVertices() const;

    std::vector<EntityVertex> createEntityVertices() const;

    std::variant<std::vector<Vertex>, std::vector<EntityVertex>> build();

   private:
    std::optional<std::vector<glm::vec3>> vertexPositions{};
    std::optional<std::vector<glm::vec2>> vertexTexCoords{};
    std::optional<std::vector<glm::vec3>> vertexNormals{};
};