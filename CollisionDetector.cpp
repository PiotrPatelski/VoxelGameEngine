#include "CollisionDetector.hpp"
#include "SteveData.hpp"
#include "WaterSystem.hpp"
#include <cmath>

namespace {
constexpr int SNAP_SEARCH_DISTANCE{3};
constexpr int MAX_GROUND_SEARCH_DISTANCE{5};
constexpr float FEET_OFFSET_FROM_ENTITY{0.1f};
constexpr float HITBOX_DETECTION_MARGIN{0.1f};
constexpr float HEAD_CLEARANCE_OFFSET{0.3f};
constexpr float DIRECTION_THRESHOLD{0.01f};
constexpr float MOVEMENT_THRESHOLD{0.001f};
constexpr float STEP_CHECK_HEIGHT{1.1f};
constexpr float STEP_CLEARANCE{0.01f};
constexpr float MAX_STEP_HEIGHT{1.1f};

bool isSolidForCollision(CubeType type) {
    return type != CubeType::NONE and not WaterSystem::isWater(type);
}

bool isStepHeightReasonable(float requiredStepHeight) {
    return (requiredStepHeight <= MAX_STEP_HEIGHT);
}
} // namespace

CollisionDetector::CollisionDetector(std::shared_ptr<Hitbox> hitboxPtr)
    : hitbox(hitboxPtr) {}

std::pair<glm::vec3, float> CollisionDetector::getHitboxBounds(
    const glm::vec3& position) const {
    const glm::vec3 center{position + Steve::hitboxOffset};
    const float bottom{center.y - Steve::hitboxScale.y / 2.0f};
    return {center, bottom};
}

std::optional<float> CollisionDetector::getDistanceToGround(
    const World& world, const glm::vec3& position) const {
    const auto [hitboxCenter, hitboxBottom] = getHitboxBounds(position);

    const int startY{static_cast<int>(std::floor(hitboxBottom))};
    const int endY{startY - MAX_GROUND_SEARCH_DISTANCE};

    for (int y{startY}; y >= endY; --y) {
        const glm::ivec3 checkPos{static_cast<int>(std::floor(hitboxCenter.x)),
                                  y,
                                  static_cast<int>(std::floor(hitboxCenter.z))};

        if (isSolidForCollision(world.getCubeTypeAtPosition(checkPos))) {
            const float groundSurfaceY{y + 1.0f};
            return hitboxBottom - groundSurfaceY;
        }
    }

    return std::nullopt;
}

bool CollisionDetector::checkCollisionAtPoints(
    const World& world, const std::vector<glm::vec3>& points) const {
    for (const auto& point : points) {
        const glm::ivec3 checkPos{static_cast<int>(std::floor(point.x)),
                                  static_cast<int>(std::floor(point.y)),
                                  static_cast<int>(std::floor(point.z))};
        if (isSolidForCollision(world.getCubeTypeAtPosition(checkPos))) {
            return true;
        }
    }
    return false;
}

bool CollisionDetector::checkGroundCollision(const World& world,
                                             const glm::vec3& position) const {
    hitbox->setPosition(position);
    return checkCollisionAtPoints(world, hitbox->getBottomFacePoints());
    ;
}

bool CollisionDetector::checkCeilingCollision(const World& world,
                                              const glm::vec3& position) const {
    hitbox->setPosition(position);
    return checkCollisionAtPoints(world, hitbox->getTopFacePoints());
    ;
}

std::vector<glm::vec3> CollisionDetector::getCollisionPoints(
    const glm::vec3& position, const glm::vec3& direction) const {
    hitbox->setPosition(position);

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

std::optional<glm::ivec3> CollisionDetector::isObstacleAt(
    const World& world, const glm::vec3& frontPosition,
    float currentHitboxBottom) const {
    for (int checkY = static_cast<int>(std::floor(currentHitboxBottom));
         checkY <=
         static_cast<int>(std::floor(currentHitboxBottom + STEP_CHECK_HEIGHT));
         checkY++) {
        const glm::ivec3 checkPos(
            static_cast<int>(std::floor(frontPosition.x)), checkY,
            static_cast<int>(std::floor(frontPosition.z)));

        const auto cubeType = world.getCubeTypeAtPosition(checkPos);
        if (isSolidForCollision(cubeType)) {
            return checkPos;
        }
    }
    return std::nullopt;
}

std::optional<float> CollisionDetector::calculateStepHeight(
    float obstacleTop, float currentHitboxBottom) const {
    const float requiredHitboxBottomY = obstacleTop + STEP_CLEARANCE;
    const float requiredStepHeight =
        requiredHitboxBottomY - currentHitboxBottom;

    if (requiredStepHeight > MAX_STEP_HEIGHT) {
        return std::nullopt;
    }

    return requiredStepHeight;
}

bool CollisionDetector::hasObstacleGroundSupport(
    const World& world, const glm::vec3& frontPosition,
    const glm::ivec3& obstaclePos) const {
    const int groundCheckY = obstaclePos.y - 1;
    const glm::ivec3 groundCheckPos(
        static_cast<int>(std::floor(frontPosition.x)), groundCheckY,
        static_cast<int>(std::floor(frontPosition.z)));
    const auto groundCubeType = world.getCubeTypeAtPosition(groundCheckPos);

    return isSolidForCollision(groundCubeType);
}

bool CollisionDetector::hasCurrentGroundSupport(
    const World& world, const glm::vec3& position,
    float currentHitboxBottom) const {
    const int currentGroundY =
        static_cast<int>(std::floor(currentHitboxBottom));
    const glm::ivec3 currentGroundPos(static_cast<int>(std::floor(position.x)),
                                      currentGroundY,
                                      static_cast<int>(std::floor(position.z)));
    const auto currentGroundType =
        world.getCubeTypeAtPosition(currentGroundPos);

    return isSolidForCollision(currentGroundType);
}

bool CollisionDetector::hasValidGroundSupport(const World& world,
                                              const glm::vec3& frontPosition,
                                              const glm::ivec3& obstaclePos,
                                              float currentHitboxBottom) const {
    const float obstacleTop = obstaclePos.y + 1.0f;
    const float requiredStepHeight =
        (obstacleTop + STEP_CLEARANCE) - currentHitboxBottom;

    const bool hasObstacleSupport =
        hasObstacleGroundSupport(world, frontPosition, obstaclePos);
    const bool hasCurrentSupport =
        hasCurrentGroundSupport(world, frontPosition, currentHitboxBottom);
    const bool isReasonableHeight = isStepHeightReasonable(requiredStepHeight);

    if (!hasObstacleSupport && !hasCurrentSupport && !isReasonableHeight) {
        return false;
    }

    return true;
}

bool CollisionDetector::hasHeadClearance(const World& world,
                                         const glm::vec3& position,
                                         const glm::vec3& direction,
                                         float requiredStepHeight) const {
    const auto headCheckPoints =
        getHeadCheckPoints(position, direction, requiredStepHeight);

    for (const auto& headPoint : headCheckPoints) {
        if (isColliding(world, headPoint)) {
            return false;
        }
    }

    return true;
}

std::vector<glm::vec3> CollisionDetector::getHeadCheckPoints(
    const glm::vec3& position, const glm::vec3& direction,
    float requiredStepHeight) const {
    const glm::vec3 steppedUpPosition =
        position + glm::vec3(0.0f, requiredStepHeight, 0.0f);
    const glm::vec3 steppedUpCenter = steppedUpPosition + Steve::hitboxOffset;

    const float headY = steppedUpCenter.y + Steve::hitboxScale.y / 2.0f;

    return {{steppedUpCenter.x, headY, steppedUpCenter.z},
            {steppedUpCenter.x + direction.x * HEAD_CLEARANCE_OFFSET, headY,
             steppedUpCenter.z + direction.z * HEAD_CLEARANCE_OFFSET}};
}

bool CollisionDetector::isColliding(const World& world,
                                    const glm::vec3& point) const {
    const glm::ivec3 checkPos(static_cast<int>(std::floor(point.x)),
                              static_cast<int>(std::floor(point.y)),
                              static_cast<int>(std::floor(point.z)));

    const auto cubeType = world.getCubeTypeAtPosition(checkPos);
    if (isSolidForCollision(cubeType)) {
        return true;
    }
    return false;
}

std::optional<float> CollisionDetector::getStepUpHeight(
    const World& world, const glm::vec3& position,
    const glm::vec3& direction) const {
    const auto [currentCenter, currentHitboxBottom] = getHitboxBounds(position);

    const glm::vec3 frontPosition =
        currentCenter +
        direction * (Steve::hitboxScale.x / 2.0f + HITBOX_DETECTION_MARGIN);

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

    if (!hasHeadClearance(world, position, direction, *requiredStepHeight)) {
        return std::nullopt;
    }
    return requiredStepHeight;
}

glm::vec3 CollisionDetector::tryHorizontalMovement(
    const World& world, const glm::vec3& position,
    const glm::vec3& horizontalMovement) const {
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

    if (isSolidForCollision(cubeType)) {
        const auto stepUpHeight =
            getStepUpHeight(world, position, horizontalDirection);
        if (stepUpHeight.has_value()) {
            return position + horizontalMovement +
                   glm::vec3(0.0f, *stepUpHeight, 0.0f);
        } else {
            return position;
        }
    }
    return position + horizontalMovement;
}

glm::vec3 CollisionDetector::tryVerticalMovement(
    const World& world, const glm::vec3& currentPosition,
    float verticalMovement) const {
    if (std::abs(verticalMovement) <= MOVEMENT_THRESHOLD) {
        return currentPosition;
    }

    const glm::vec3 testPosition =
        currentPosition + glm::vec3(0.0f, verticalMovement, 0.0f);

    const bool movingUp = (verticalMovement > 0.0f);

    if (movingUp) {
        if (checkCeilingCollision(world, testPosition)) {
            return currentPosition;
        }
    } else {
        if (checkGroundCollision(world, testPosition)) {
            return currentPosition;
        }
    }

    return testPosition;
}

std::optional<float> CollisionDetector::findGroundSurfaceForSnapping(
    const World& world, const glm::vec3& position) const {
    const glm::vec3 center = position + Steve::hitboxOffset;

    const int startY = static_cast<int>(center.y);
    const int endY = startY - SNAP_SEARCH_DISTANCE;

    for (int y = startY; y >= endY; y--) {
        const glm::ivec3 checkPos(static_cast<int>(std::floor(center.x)), y,
                                  static_cast<int>(std::floor(center.z)));

        const auto cubeType = world.getCubeTypeAtPosition(checkPos);
        if (isSolidForCollision(cubeType)) {
            return static_cast<float>(y + 1);
        }
    }

    return std::nullopt;
}
