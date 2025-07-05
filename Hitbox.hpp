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

    std::vector<glm::vec3> getBottomFacePoints() const;
    std::vector<glm::vec3> getFrontFacePoints() const;
    std::vector<glm::vec3> getBackFacePoints() const;
    std::vector<glm::vec3> getLeftFacePoints() const;
    std::vector<glm::vec3> getRightFacePoints() const;
    std::vector<glm::vec3> getTopFacePoints() const;
    glm::vec3 getMinBounds() const;
    glm::vec3 getMaxBounds() const;

   private:
    static constexpr float COLLISION_POINT_SHRINK_FACTOR = 0.9f;

    void setupBuffers();
    glm::vec3 getHitboxCenter() const;
    glm::vec3 getHalfScale() const;
    glm::vec3 getShrunkHalfScale() const;

    std::vector<glm::vec3> createHorizontalFacePoints(
        const glm::vec3& center, const glm::vec3& shrunkHalfScale,
        float fixedY) const;
    std::vector<glm::vec3> createVerticalFacePointsZ(
        const glm::vec3& center, const glm::vec3& shrunkHalfScale,
        float fixedZ) const;
    std::vector<glm::vec3> createVerticalFacePointsX(
        const glm::vec3& center, const glm::vec3& shrunkHalfScale,
        float fixedX) const;

    glm::vec3 position{};
    glm::vec3 offset{};
    glm::vec3 scale{};

    GLuint vertexArrayObject{0};
    GLuint vertexBufferObject{0};
    GLuint elementBufferObject{0};

    std::unique_ptr<Shader> shader{nullptr};
    glm::mat4 rotation{1.0f};
};
