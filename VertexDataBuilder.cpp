#include <vector>
#include "VertexDataBuilder.hpp"

VertexDataBuilder& VertexDataBuilder::withTexCoords(
    const std::vector<glm::vec2>& texCoords) {
    vertexTexCoords = texCoords;
    return *this;
}
VertexDataBuilder& VertexDataBuilder::withNormals(
    const std::vector<glm::vec3>& normals) {
    vertexNormals = normals;
    return *this;
}

std::vector<Vertex> VertexDataBuilder::createCubeVertices() const {
    assert(vertexTexCoords and vertexNormals and
           "Vertex texture coordinates, and normals must be provided.");
    std::vector<Vertex> vertices;
    const auto size = cubeVertices.size();
    vertices.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        vertices.push_back(
            {cubeVertices[i], (*vertexTexCoords)[i], (*vertexNormals)[i]});
    }
    return vertices;
}

std::vector<EntityVertex> VertexDataBuilder::createEntityVertices() const {
    assert(vertexTexCoords and "Vertex texture coordinates must be provided.");
    std::vector<EntityVertex> vertices;
    const auto size = cubeVertices.size();
    vertices.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        vertices.push_back({cubeVertices[i], (*vertexTexCoords)[i]});
    }
    return vertices;
}

std::variant<std::vector<Vertex>, std::vector<EntityVertex>>
VertexDataBuilder::build() {
    if (vertexNormals) {
        return createCubeVertices();
    } else {
        return createEntityVertices();
    }
    vertexPositions.reset();
    vertexTexCoords.reset();
    vertexNormals.reset();
}

std::vector<Vertex> createCubeVertexVector() {
    VertexDataBuilder builder;
    builder.withTexCoords(wholeFileTexCoords).withNormals(cubeNormals);
    return builder.createCubeVertices();
}

std::vector<EntityVertex> createEntityVertexVector(
    const std::vector<glm::vec2>& texCoords) {
    VertexDataBuilder builder;
    builder.withTexCoords(texCoords);
    return builder.createEntityVertices();
}