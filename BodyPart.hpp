#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "VertexData.hpp"
#include "Shader.hpp"

class BodyPart {
   public:
    BodyPart(const std::vector<glm::vec2>& texCoords, const glm::vec3& offset,
             const glm::vec3& scale, GLuint texture);
    ~BodyPart();

    void initVertexAttributes();
    void render(const glm::mat4& transform, Shader& shader,
                int elementBufferObject);

   private:
    std::vector<EntityVertex> vertices{};
    glm::vec3 offset{};
    glm::vec3 scale{};
    GLuint textureID{};
    GLuint vertexArrayObject{};
    GLuint vertexBufferObject{};
};
