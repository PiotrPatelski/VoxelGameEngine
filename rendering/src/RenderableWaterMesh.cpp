#include "RenderableWaterMesh.hpp"
#include <utility>
#include "VertexData.hpp"

RenderableWaterMesh::RenderableWaterMesh(const CpuWaterMesh& data)
    : indexCount(static_cast<unsigned int>(data.getIndices().size())),
      waterType(data.getWaterType()),
      center(data.getCenter()) {
    const auto& meshVertices = data.getVertices();
    const auto& meshIndices = data.getIndices();

    if (not meshVertices.empty() and not meshIndices.empty()) {
        glGenVertexArrays(1, &VertexArrayObject);
        glGenBuffers(1, &VertexBufferObject);
        glGenBuffers(1, &ElementBufferObject);

        glBindVertexArray(VertexArrayObject);

        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, meshVertices.size() * sizeof(Vertex),
                     meshVertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     meshIndices.size() * sizeof(unsigned int),
                     meshIndices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void*)offsetof(Vertex, texCoord));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
}

RenderableWaterMesh::~RenderableWaterMesh() { cleanup(); }

RenderableWaterMesh::RenderableWaterMesh(RenderableWaterMesh&& other) noexcept
    : VertexArrayObject(other.VertexArrayObject),
      VertexBufferObject(other.VertexBufferObject),
      ElementBufferObject(other.ElementBufferObject),
      indexCount(other.indexCount),
      waterType(other.waterType),
      center(other.center) {
    other.VertexArrayObject = 0;
    other.VertexBufferObject = 0;
    other.ElementBufferObject = 0;
    other.indexCount = 0;
}

RenderableWaterMesh& RenderableWaterMesh::operator=(
    RenderableWaterMesh&& other) noexcept {
    if (this != &other) {
        cleanup();

        VertexArrayObject = other.VertexArrayObject;
        VertexBufferObject = other.VertexBufferObject;
        ElementBufferObject = other.ElementBufferObject;
        indexCount = other.indexCount;
        waterType = other.waterType;
        center = other.center;

        other.VertexArrayObject = 0;
        other.VertexBufferObject = 0;
        other.ElementBufferObject = 0;
        other.indexCount = 0;
    }
    return *this;
}

void RenderableWaterMesh::render() const {
    if (indexCount > 0 and VertexArrayObject != 0) {
        glBindVertexArray(VertexArrayObject);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void RenderableWaterMesh::cleanup() {
    if (VertexArrayObject != 0) {
        glDeleteVertexArrays(1, &VertexArrayObject);
        VertexArrayObject = 0;
    }
    if (VertexBufferObject != 0) {
        glDeleteBuffers(1, &VertexBufferObject);
        VertexBufferObject = 0;
    }
    if (ElementBufferObject != 0) {
        glDeleteBuffers(1, &ElementBufferObject);
        ElementBufferObject = 0;
    }
}
