#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "Shader.hpp"
#include "VertexData.hpp"

class Entity {
   public:
    Entity();
    ~Entity();

    void render();
    void update(const glm::mat4& view, const glm::mat4& projection);

   private:
    GLuint vertexArrayObject{0};
    GLuint vertexBufferObject{0};
    GLuint elementBufferObject{0};
    std::vector<glm::vec3> entityVertices{};
    glm::mat4 viewMatrix{};
    glm::mat4 projectionMatrix{};
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    std::unique_ptr<Shader> shader{nullptr};

    void renderCube(const glm::vec3& position, const glm::vec3& scale);
    void initVertexAttributes();
};
