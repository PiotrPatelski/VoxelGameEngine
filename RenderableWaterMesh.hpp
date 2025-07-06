#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include <glad/glad.h>
#include "Cube.hpp"
#include "CpuWaterMesh.hpp"

class RenderableWaterMesh {
   public:
    explicit RenderableWaterMesh(const CpuWaterMesh& data);
    ~RenderableWaterMesh();

    RenderableWaterMesh(const RenderableWaterMesh&) = delete;
    RenderableWaterMesh& operator=(const RenderableWaterMesh&) = delete;
    RenderableWaterMesh(RenderableWaterMesh&& other) noexcept;
    RenderableWaterMesh& operator=(RenderableWaterMesh&& other) noexcept;

    void render() const;
    CubeType getWaterType() const { return waterType; }
    glm::vec3 getCenter() const { return center; }
    bool isEmpty() const { return indexCount == 0; }

   private:
    void cleanup();

    GLuint VertexArrayObject{0};
    GLuint VertexBufferObject{0};
    GLuint ElementBufferObject{0};
    unsigned int indexCount{0};
    CubeType waterType{CubeType::WATER_SOURCE};
    glm::vec3 center{0.0f};
};
