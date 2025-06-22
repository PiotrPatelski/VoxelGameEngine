#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "Shader.hpp"

class Hitbox {
   public:
    Hitbox(const glm::vec3& position, const glm::vec3& offset,
           const glm::vec3& scale);
    ~Hitbox();
    glm::vec3 getBottomFacePosition() const;
    glm::vec3 getFrontFacePosition() const;
    void updateShaders(const glm::mat4& view, const glm::mat4& projection);
    void setRotation(const glm::mat4& rotationMatrix);
    void render();
    void setPosition(const glm::vec3& newPosition);

   private:
    void setupBuffers();

    glm::vec3 position{};
    glm::vec3 offset{};
    glm::vec3 scale{};

    GLuint vertexArrayObject{0};
    GLuint vertexBufferObject{0};
    GLuint elementBufferObject{0};

    std::unique_ptr<Shader> shader{nullptr};
    glm::mat4 rotation{1.0f};
};
