#include "PhysicsComponent.hpp"
#include "SteveData.hpp"
#include <cmath>
#include <iostream>

namespace {
static constexpr int SNAP_SEARCH_DISTANCE = 3;
static constexpr float FEET_OFFSET_FROM_ENTITY = 0.1f;
} // namespace

PhysicsComponent::PhysicsComponent(const glm::vec3& initialPosition,
                                   std::shared_ptr<Hitbox> hitboxPtr)
    : position(initialPosition), hitbox(hitboxPtr) {}

void PhysicsComponent::setPosition(const glm::vec3& newPosition) {
    position = newPosition;
    if (hitbox) {
        hitbox->setPosition(position);
    }
}

std::pair<glm::vec3, float> PhysicsComponent::getHitboxBounds() const {
    const glm::vec3 center{position + Steve::hitboxOffset};
    const float bottom{center.y - Steve::hitboxScale.y / 2.0f};
    return {center, bottom};
}

std::optional<float> PhysicsComponent::getDistanceToGround(
    const World& world) const {
    static constexpr int MAX_SEARCH_DISTANCE{5};

    const auto [hitboxCenter, hitboxBottom] = getHitboxBounds();

    const int startY{static_cast<int>(std::floor(hitboxBottom))};
    const int endY{startY - MAX_SEARCH_DISTANCE};

    for (int y{startY}; y >= endY; --y) {
        const glm::ivec3 checkPos{static_cast<int>(std::floor(hitboxCenter.x)),
                                  y,
                                  static_cast<int>(std::floor(hitboxCenter.z))};

        if (world.getCubeTypeAtPosition(checkPos) != CubeType::NONE) {
            const float groundSurfaceY{y + 1.0f};
            return hitboxBottom - groundSurfaceY;
        }
    }

    return std::nullopt;
}

bool PhysicsComponent::checkCollisionAtPoints(
    const World& world, const std::vector<glm::vec3>& points) const {
    for (const auto& point : points) {
        const glm::ivec3 checkPos{static_cast<int>(std::floor(point.x)),
                                  static_cast<int>(std::floor(point.y)),
                                  static_cast<int>(std::floor(point.z))};
        if (world.getCubeTypeAtPosition(checkPos) != CubeType::NONE) {
            return true;
        }
    }
    return false;
}

bool PhysicsComponent::checkGroundCollision(const World& world) const {
    return checkCollisionAtPoints(world, hitbox->getBottomFacePoints());
}

bool PhysicsComponent::checkCeilingCollision(const World& world) const {
    return checkCollisionAtPoints(world, hitbox->getTopFacePoints());
}

std::vector<glm::vec3> PhysicsComponent::getCollisionPoints(
    const glm::vec3& direction) const {
    static constexpr float DIRECTION_THRESHOLD{0.01f};

    if (direction.x > DIRECTION_THRESHOLD) {
        return hitbox->getRightFacePoints();
    } else if (direction.x < -DIRECTION_THRESHOLD) {
        return hitbox->getLeftFacePoints();
    } else if (direction.z > DIRECTION_THRESHOLD) {
        return hitbox->getFrontFacePoints();
    } else if (direction.z < -DIRECTION_THRESHOLD) {
        return hitbox->getBackFacePoints();
    }

    return {};
}

bool PhysicsComponent::checkHorizontalCollision(
    const World& world, const glm::vec3& direction) const {
    return checkCollisionAtPoints(world, getCollisionPoints(direction));
}

std::optional<glm::ivec3> PhysicsComponent::isObstacleAt(
    const World& world, const glm::vec3& frontPosition,
    float currentHitboxBottom) const {
    static constexpr float STEP_CHECK_HEIGHT{1.1f};

    for (int checkY = static_cast<int>(std::floor(currentHitboxBottom));
         checkY <=
         static_cast<int>(std::floor(currentHitboxBottom + STEP_CHECK_HEIGHT));
         checkY++) {
        const glm::ivec3 checkPos(
            static_cast<int>(std::floor(frontPosition.x)), checkY,
            static_cast<int>(std::floor(frontPosition.z)));

        const auto cubeType = world.getCubeTypeAtPosition(checkPos);
        if (cubeType != CubeType::NONE) {
            return checkPos;
        }
    }
    return std::nullopt;
}

std::optional<float> PhysicsComponent::calculateStepHeight(
    float obstacleTop, float currentHitboxBottom) const {
    static constexpr float CLEARANCE{0.01f};
    static constexpr float MAX_STEP_HEIGHT{1.1f};

    const float requiredHitboxBottomY = obstacleTop + CLEARANCE;
    const float requiredStepHeight =
        requiredHitboxBottomY - currentHitboxBottom;

    if (requiredStepHeight > MAX_STEP_HEIGHT) {
        return std::nullopt;
    }

    return requiredStepHeight;
}

bool PhysicsComponent::hasObstacleGroundSupport(
    const World& world, const glm::vec3& frontPosition,
    const glm::ivec3& obstaclePos) const {
    const int groundCheckY = obstaclePos.y - 1;
    const glm::ivec3 groundCheckPos(
        static_cast<int>(std::floor(frontPosition.x)), groundCheckY,
        static_cast<int>(std::floor(frontPosition.z)));
    const auto groundCubeType = world.getCubeTypeAtPosition(groundCheckPos);

    return (groundCubeType != CubeType::NONE);
}

bool PhysicsComponent::hasCurrentGroundSupport(
    const World& world, float currentHitboxBottom) const {
    const int currentGroundY =
        static_cast<int>(std::floor(currentHitboxBottom));
    const glm::ivec3 currentGroundPos(static_cast<int>(std::floor(position.x)),
                                      currentGroundY,
                                      static_cast<int>(std::floor(position.z)));
    const auto currentGroundType =
        world.getCubeTypeAtPosition(currentGroundPos);

    return (currentGroundType != CubeType::NONE);
}

bool PhysicsComponent::isStepHeightReasonable(float requiredStepHeight) const {
    static constexpr float MAX_STEP_HEIGHT{1.0f};
    return (requiredStepHeight <= MAX_STEP_HEIGHT);
}

bool PhysicsComponent::hasValidGroundSupport(const World& world,
                                             const glm::vec3& frontPosition,
                                             const glm::ivec3& obstaclePos,
                                             float currentHitboxBottom) const {
    const float obstacleTop = obstaclePos.y + 1.0f;
    const float requiredStepHeight =
        (obstacleTop + 0.01f) - currentHitboxBottom;

    const bool hasObstacleSupport =
        hasObstacleGroundSupport(world, frontPosition, obstaclePos);
    const bool hasCurrentSupport =
        hasCurrentGroundSupport(world, currentHitboxBottom);
    const bool isReasonableHeight = isStepHeightReasonable(requiredStepHeight);

    if (!hasObstacleSupport && !hasCurrentSupport && !isReasonableHeight) {
        return false;
    }

    return true;
}

bool PhysicsComponent::hasHeadClearance(const World& world,
                                        const glm::vec3& direction,
                                        float requiredStepHeight) const {
    const auto headCheckPoints =
        getHeadCheckPoints(direction, requiredStepHeight);

    for (const auto& headPoint : headCheckPoints) {
        if (isColliding(world, headPoint)) {
            return false;
        }
    }

    return true;
}

std::vector<glm::vec3> PhysicsComponent::getHeadCheckPoints(
    const glm::vec3& direction, float requiredStepHeight) const {
    const glm::vec3 steppedUpPosition =
        position + glm::vec3(0.0f, requiredStepHeight, 0.0f);
    const glm::vec3 steppedUpCenter = steppedUpPosition + Steve::hitboxOffset;

    const float headY = steppedUpCenter.y + Steve::hitboxScale.y / 2.0f;

    return {{steppedUpCenter.x, headY, steppedUpCenter.z},
            {steppedUpCenter.x + direction.x * 0.3f, headY,
             steppedUpCenter.z + direction.z * 0.3f}};
}

bool PhysicsComponent::isColliding(const World& world,
                                   const glm::vec3& point) const {
    const glm::ivec3 checkPos(static_cast<int>(std::floor(point.x)),
                              static_cast<int>(std::floor(point.y)),
                              static_cast<int>(std::floor(point.z)));

    const auto cubeType = world.getCubeTypeAtPosition(checkPos);
    if (cubeType != CubeType::NONE) {
        return true;
    }
    return false;
}

std::optional<float> PhysicsComponent::getStepUpHeight(
    const World& world, const glm::vec3& direction) const {
    const auto [currentCenter, currentHitboxBottom] = getHitboxBounds();

    const glm::vec3 frontPosition =
        currentCenter + direction * (Steve::hitboxScale.x / 2.0f + 0.1f);

    const auto obstaclePos =
        isObstacleAt(world, frontPosition, currentHitboxBottom);
    if (!obstaclePos) {
        return std::nullopt;
    }

    const float obstacleTop = obstaclePos->y + 1.0f;
    const auto requiredStepHeight =
        calculateStepHeight(obstacleTop, currentHitboxBottom);
    if (!requiredStepHeight) {
        return std::nullopt;
    }

    if (!hasValidGroundSupport(world, frontPosition, *obstaclePos,
                               currentHitboxBottom)) {
        return std::nullopt;
    }

    if (!hasHeadClearance(world, direction, *requiredStepHeight)) {
        return std::nullopt;
    }
    return requiredStepHeight;
}

glm::vec3 PhysicsComponent::tryHorizontalMovement(
    const World& world, const glm::vec3& horizontalMovement) const {
    static constexpr float MOVEMENT_THRESHOLD{0.001f};

    if (glm::length(horizontalMovement) <= MOVEMENT_THRESHOLD) {
        return position;
    }

    const glm::vec3 horizontalDirection = glm::normalize(horizontalMovement);
    const glm::vec3 currentCenter = position + Steve::hitboxOffset;

    const glm::vec3 frontCheckPosition =
        currentCenter +
        horizontalDirection * (Steve::hitboxScale.x / 2.0f + 0.1f);
    const glm::ivec3 frontCheckPos(
        static_cast<int>(std::floor(frontCheckPosition.x)),
        static_cast<int>(std::floor(frontCheckPosition.y)),
        static_cast<int>(std::floor(frontCheckPosition.z)));

    const auto cubeType = world.getCubeTypeAtPosition(frontCheckPos);

    if (cubeType != CubeType::NONE) {
        const auto stepUpHeight = getStepUpHeight(world, horizontalDirection);
        if (stepUpHeight.has_value()) {
            return position + horizontalMovement +
                   glm::vec3(0.0f, *stepUpHeight, 0.0f);
        } else {
            return position;
        }
    }
    return position + horizontalMovement;
}

glm::vec3 PhysicsComponent::tryVerticalMovement(
    const World& world, const glm::vec3& currentPosition,
    float verticalMovement) const {
    static constexpr float MOVEMENT_THRESHOLD{0.001f};

    if (std::abs(verticalMovement) <= MOVEMENT_THRESHOLD) {
        return currentPosition;
    }

    const glm::vec3 testPosition =
        currentPosition + glm::vec3(0.0f, verticalMovement, 0.0f);
    const glm::vec3 testCenter = testPosition + Steve::hitboxOffset;

    const bool movingUp = (verticalMovement > 0.0f);
    if (checkVerticalCollision(world, testCenter, movingUp)) {
        return currentPosition;
    }

    return testPosition;
}

bool PhysicsComponent::checkVerticalCollision(const World& world,
                                              const glm::vec3& testCenter,
                                              bool movingUp) const {
    float resultY;

    if (movingUp) {
        resultY = testCenter.y + Steve::hitboxScale.y / 2.0f;
    } else {
        resultY = testCenter.y - Steve::hitboxScale.y / 2.0f;
    }

    const auto checkPos =
        glm::ivec3(static_cast<int>(std::floor(testCenter.x)),
                   static_cast<int>(std::floor(resultY)),
                   static_cast<int>(std::floor(testCenter.z)));

    const auto cubeType = world.getCubeTypeAtPosition(checkPos);
    return (cubeType != CubeType::NONE);
}

glm::vec3 PhysicsComponent::tryMove(const World& world,
                                    const glm::vec3& movement) const {
    const glm::vec3 horizontalMovement(movement.x, 0.0f, movement.z);
    glm::vec3 newPosition = tryHorizontalMovement(world, horizontalMovement);

    if (std::abs(movement.y) > 0.001f) {
        newPosition = tryVerticalMovement(world, newPosition, movement.y);
    }

    return newPosition;
}

std::optional<float> PhysicsComponent::findGroundSurfaceForSnapping(
    const World& world) const {
    const glm::vec3 center = position + Steve::hitboxOffset;

    const int startY = static_cast<int>(center.y);
    const int endY = startY - SNAP_SEARCH_DISTANCE;

    for (int y = startY; y >= endY; y--) {
        const glm::ivec3 checkPos(static_cast<int>(std::floor(center.x)), y,
                                  static_cast<int>(std::floor(center.z)));

        const auto cubeType = world.getCubeTypeAtPosition(checkPos);
        if (cubeType != CubeType::NONE) {
            return static_cast<float>(y + 1);
        }
    }

    return std::nullopt;
}

float PhysicsComponent::calculateEntityYFromGroundSurface(
    float groundSurfaceY) const {
    return groundSurfaceY - FEET_OFFSET_FROM_ENTITY;
}

void PhysicsComponent::snapToGround(const World& world) {
    const auto groundSurfaceY = findGroundSurfaceForSnapping(world);
    if (groundSurfaceY.has_value()) {
        position.y = calculateEntityYFromGroundSurface(*groundSurfaceY);
    }
}

bool PhysicsComponent::moveForward(const World& world,
                                   float currentDirectionAngle, float speed) {
    const auto forwardDirection =
        glm::normalize(glm::vec3(std::sin(currentDirectionAngle), 0.0f,
                                 std::cos(currentDirectionAngle)));
    const auto movement = forwardDirection * speed;

    const auto previousPosition = position;

    const auto newPosition = tryMove(world, movement);

    if (newPosition != previousPosition) {
        setPosition(newPosition);

        const auto groundDistance = getDistanceToGround(world);
        if (groundDistance.has_value() && *groundDistance >= 0.0f &&
            *groundDistance <= 0.1f) {
            snapToGround(world);
        }

        return true;
    }

    return false;
}

bool PhysicsComponent::performFallMovement(const World& world) {
    static constexpr float FALL_SPEED{-0.1f};

    const auto fallMovement = glm::vec3(0.0f, FALL_SPEED, 0.0f);
    const auto newPosition = tryMove(world, fallMovement);

    if (newPosition != position) {
        setPosition(newPosition);
        return true;
    }

    return false;
}

bool PhysicsComponent::shouldSnapToGround(float groundDistance) const {
    static constexpr float SNAP_THRESHOLD{0.05f};
    return (groundDistance <= SNAP_THRESHOLD);
}

bool PhysicsComponent::updateFalling(const World& world) {
    const auto groundDistance = getDistanceToGround(world);

    if (!groundDistance.has_value()) {
        return performFallMovement(world);
    } else if (shouldSnapToGround(*groundDistance)) {
        snapToGround(world);
        return true;
    } else {
        return performFallMovement(world);
    }
}
