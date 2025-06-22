#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "Shader.hpp"
#include "VertexData.hpp"
#include "TextureManager.hpp"
#include "BodyPart.hpp"
#include "Hitbox.hpp"
#include "World.hpp"

class Entity {
   public:
    Entity();
    ~Entity();

    void render();
    void update(const glm::mat4& view, const glm::mat4& projection,
                const World& world);

    glm::vec3 getPosition() const { return entityPosition; }

    void moveForward(const World& world);

   private:
    void createLimbs();
    void updateFallingMovement(const World& world);
    void updateShaders(const glm::mat4& view, const glm::mat4& projection);
    void updateMoveAnimation();
    void updateDirection();
    void applyAnimationTransforms(float currentAnimationAngle,
                                  const glm::mat4& bodyRotation);
    void applyRotationTransforms(const glm::mat4& bodyRotation,
                                 const glm::mat4& bodyOffsetTranslation);
    void rotate();
    glm::mat4 createLimbTransform(const glm::vec3& offset, float angle,
                                  const glm::mat4& bodyRotation) const;
    std::unique_ptr<BodyPart> head{nullptr};
    std::unique_ptr<BodyPart> body{nullptr};
    std::unique_ptr<BodyPart> leftArm{nullptr};
    std::unique_ptr<BodyPart> rightArm{nullptr};
    std::unique_ptr<BodyPart> leftLeg{nullptr};
    std::unique_ptr<BodyPart> rightLeg{nullptr};
    std::unique_ptr<Hitbox> hitbox{nullptr};
    GLuint elementBufferObject{0};
    glm::vec3 entityPosition{0.0f, 0.0f, 0.0f};
    GLuint textureID{0};
    std::unique_ptr<Shader> shader{nullptr};
    glm::mat4 headTransform{};
    glm::mat4 bodyTransform{};
    glm::mat4 leftArmTransform{};
    glm::mat4 rightArmTransform{};
    glm::mat4 leftLegTransform{};
    glm::mat4 rightLegTransform{};
    float directionAngle{0.0f};
    float lastTime{0.0f};
    bool isMoving{false};
};
