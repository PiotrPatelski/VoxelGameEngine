#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include "World.hpp"
#include "Hitbox.hpp"
#include "CollisionDetector.hpp"

class PhysicsComponent {
   public:
    PhysicsComponent(const glm::vec3& initialPosition,
                     std::shared_ptr<Hitbox> hitboxPtr);
    ~PhysicsComponent() = default;

    glm::vec3 getPosition() const { return position; }
    void setPosition(const glm::vec3& newPosition);

    bool moveForward(const World& world, float currentDirectionAngle,
                     float speed = 0.05f);
    bool updateFalling(const World& world);

    void setDirectionAngle(float angle) { directionAngle = angle; }
    float getDirectionAngle() const { return directionAngle; }

   private:
    glm::vec3 tryMove(const World& world, const glm::vec3& movement) const;
    void snapToGround(const World& world);

    bool performFallMovement(const World& world);
    bool shouldSnapToGround(float groundDistance) const;

    glm::vec3 position{};
    std::shared_ptr<Hitbox> hitbox{nullptr};
    std::unique_ptr<CollisionDetector> collisionDetector{nullptr};
    float directionAngle{0.0f};
};
