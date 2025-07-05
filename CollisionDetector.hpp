#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include <vector>
#include "World.hpp"
#include "Hitbox.hpp"

class CollisionDetector {
   public:
    CollisionDetector(std::shared_ptr<Hitbox> hitboxPtr);
    ~CollisionDetector() = default;

    std::optional<float> getDistanceToGround(const World& world,
                                             const glm::vec3& position) const;
    std::optional<float> findGroundSurfaceForSnapping(
        const World& world, const glm::vec3& position) const;

    glm::vec3 tryHorizontalMovement(const World& world,
                                    const glm::vec3& position,
                                    const glm::vec3& horizontalMovement) const;
    glm::vec3 tryVerticalMovement(const World& world,
                                  const glm::vec3& currentPosition,
                                  float verticalMovement) const;

   private:
    bool checkGroundCollision(const World& world,
                              const glm::vec3& position) const;
    bool checkCeilingCollision(const World& world,
                               const glm::vec3& position) const;

    std::optional<float> getStepUpHeight(const World& world,
                                         const glm::vec3& position,
                                         const glm::vec3& direction) const;

    std::vector<glm::vec3> getCollisionPoints(const glm::vec3& position,
                                              const glm::vec3& direction) const;
    std::pair<glm::vec3, float> getHitboxBounds(
        const glm::vec3& position) const;

    bool checkCollisionAtPoints(const World& world,
                                const std::vector<glm::vec3>& points) const;
    bool isColliding(const World& world, const glm::vec3& point) const;

    std::optional<glm::ivec3> isObstacleAt(const World& world,
                                           const glm::vec3& frontPosition,
                                           float currentHitboxBottom) const;
    std::optional<float> calculateStepHeight(float obstacleTop,
                                             float currentHitboxBottom) const;
    bool hasValidGroundSupport(const World& world,
                               const glm::vec3& frontPosition,
                               const glm::ivec3& obstaclePos,
                               float currentHitboxBottom) const;
    bool hasHeadClearance(const World& world, const glm::vec3& position,
                          const glm::vec3& direction,
                          float requiredStepHeight) const;

    bool hasObstacleGroundSupport(const World& world,
                                  const glm::vec3& frontPosition,
                                  const glm::ivec3& obstaclePos) const;
    bool hasCurrentGroundSupport(const World& world, const glm::vec3& position,
                                 float currentHitboxBottom) const;

    std::vector<glm::vec3> getHeadCheckPoints(const glm::vec3& position,
                                              const glm::vec3& direction,
                                              float requiredStepHeight) const;

    std::shared_ptr<Hitbox> hitbox{nullptr};
};
