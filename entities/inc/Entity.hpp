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
#include "PhysicsComponent.hpp"

class Entity {
   public:
    Entity(const glm::vec3& initialPosition);
    ~Entity();

    void render();
    void updatePhysics(const World& world);
    void updateRendering(const Camera& camera);

    glm::vec3 getPosition() const { return physicsComponent->getPosition(); }
    void setPosition(const glm::vec3& position);

    void moveForward(const World& world);

   private:
    void createLimbs();
    void updateFallingMovement(const World& world);
    void updateShaders(const Camera& camera);
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
    std::shared_ptr<Hitbox> hitbox{nullptr};
    std::unique_ptr<PhysicsComponent> physicsComponent{nullptr};
    GLuint elementBufferObject{0};
    GLuint textureID{0};
    std::unique_ptr<Shader> shader{nullptr};
    glm::mat4 headTransform{};
    glm::mat4 bodyTransform{};
    glm::mat4 leftArmTransform{};
    glm::mat4 rightArmTransform{};
    glm::mat4 leftLegTransform{};
    glm::mat4 rightLegTransform{};
    float lastTime{0.0f};
    bool isMoving{false};
};
