#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include "World.hpp"
#include "Hitbox.hpp"

class PhysicsComponent {
   public:
    PhysicsComponent(const glm::vec3& initialPosition,
                     std::shared_ptr<Hitbox> hitboxPtr);
    ~PhysicsComponent() = default;

    glm::vec3 tryMove(const World& world, const glm::vec3& movement) const;
    void snapToGround(const World& world);

    std::optional<float> getDistanceToGround(const World& world) const;
    bool checkGroundCollision(const World& world) const;
    bool checkCeilingCollision(const World& world) const;
    bool checkHorizontalCollision(const World& world,
                                  const glm::vec3& direction) const;
    std::optional<float> getStepUpHeight(const World& world,
                                         const glm::vec3& direction) const;

    glm::vec3 getPosition() const { return position; }
    void setPosition(const glm::vec3& newPosition);

    bool moveForward(const World& world, float currentDirectionAngle,
                     float speed = 0.05f);
    bool updateFalling(const World& world);

    void setDirectionAngle(float angle) { directionAngle = angle; }
    float getDirectionAngle() const { return directionAngle; }

   private:
    std::pair<glm::vec3, float> getHitboxBounds() const;
    bool checkCollisionAtPoints(const World& world,
                                const std::vector<glm::vec3>& points) const;
    std::vector<glm::vec3> getCollisionPoints(const glm::vec3& direction) const;

    std::optional<float> findGroundSurfaceForSnapping(const World& world) const;
    float calculateEntityYFromGroundSurface(float groundSurfaceY) const;

    std::optional<glm::ivec3> isObstacleAt(const World& world,
                                           const glm::vec3& frontPosition,
                                           float currentHitboxBottom) const;
    std::optional<float> calculateStepHeight(float obstacleTop,
                                             float currentHitboxBottom) const;
    bool hasValidGroundSupport(const World& world,
                               const glm::vec3& frontPosition,
                               const glm::ivec3& obstaclePos,
                               float currentHitboxBottom) const;
    bool hasHeadClearance(const World& world, const glm::vec3& direction,
                          float requiredStepHeight) const;

    bool hasObstacleGroundSupport(const World& world,
                                  const glm::vec3& frontPosition,
                                  const glm::ivec3& obstaclePos) const;
    bool hasCurrentGroundSupport(const World& world,
                                 float currentHitboxBottom) const;
    bool isStepHeightReasonable(float requiredStepHeight) const;

    std::vector<glm::vec3> getHeadCheckPoints(const glm::vec3& direction,
                                              float requiredStepHeight) const;
    bool isColliding(const World& world, const glm::vec3& point) const;

    glm::vec3 tryHorizontalMovement(const World& world,
                                    const glm::vec3& horizontalMovement) const;
    glm::vec3 tryVerticalMovement(const World& world,
                                  const glm::vec3& currentPosition,
                                  float verticalMovement) const;
    bool checkVerticalCollision(const World& world, const glm::vec3& testCenter,
                                bool movingUp) const;

    bool performFallMovement(const World& world);
    bool shouldSnapToGround(float groundDistance) const;

    glm::vec3 position{};
    std::shared_ptr<Hitbox> hitbox{nullptr};
    float directionAngle{0.0f};
};
