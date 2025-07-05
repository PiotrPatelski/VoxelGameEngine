#include "PhysicsComponent.hpp"
#include "CollisionDetector.hpp"
#include "SteveData.hpp"
#include <cmath>
#include <iostream>

namespace {
constexpr float MOVEMENT_THRESHOLD{0.001f};
constexpr float GROUND_SNAP_DISTANCE_THRESHOLD{0.1f};
constexpr float FALL_SPEED{-0.1f};
constexpr float SNAP_THRESHOLD{0.05f};
constexpr float FEET_OFFSET_FROM_ENTITY{0.1f};

float calculateEntityYFromGroundSurface(float groundSurfaceY) {
    return groundSurfaceY - FEET_OFFSET_FROM_ENTITY;
}
} // namespace

PhysicsComponent::PhysicsComponent(const glm::vec3& initialPosition,
                                   std::shared_ptr<Hitbox> hitboxPtr)
    : position(initialPosition),
      hitbox(hitboxPtr),
      collisionDetector(std::make_unique<CollisionDetector>(hitboxPtr)) {}

void PhysicsComponent::setPosition(const glm::vec3& newPosition) {
    position = newPosition;
    if (hitbox) {
        hitbox->setPosition(position);
    }
}

glm::vec3 PhysicsComponent::tryMove(const World& world,
                                    const glm::vec3& movement) const {
    const glm::vec3 horizontalMovement(movement.x, 0.0f, movement.z);
    glm::vec3 newPosition = collisionDetector->tryHorizontalMovement(
        world, position, horizontalMovement);

    if (std::abs(movement.y) > MOVEMENT_THRESHOLD) {
        newPosition = collisionDetector->tryVerticalMovement(world, newPosition,
                                                             movement.y);
    }

    return newPosition;
}

void PhysicsComponent::snapToGround(const World& world) {
    const auto groundSurfaceY =
        collisionDetector->findGroundSurfaceForSnapping(world, position);
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

        const auto groundDistance =
            collisionDetector->getDistanceToGround(world, position);
        if (groundDistance.has_value() && *groundDistance >= 0.0f &&
            *groundDistance <= GROUND_SNAP_DISTANCE_THRESHOLD) {
            snapToGround(world);
        }

        return true;
    }

    return false;
}

bool PhysicsComponent::performFallMovement(const World& world) {
    const auto fallMovement = glm::vec3(0.0f, FALL_SPEED, 0.0f);
    const auto newPosition = tryMove(world, fallMovement);

    if (newPosition != position) {
        setPosition(newPosition);
        return true;
    }

    return false;
}

bool PhysicsComponent::shouldSnapToGround(float groundDistance) const {
    return (groundDistance <= SNAP_THRESHOLD);
}

bool PhysicsComponent::updateFalling(const World& world) {
    const auto groundDistance =
        collisionDetector->getDistanceToGround(world, position);

    if (groundDistance && shouldSnapToGround(*groundDistance)) {
        snapToGround(world);
        return true;
    }
    return performFallMovement(world);
}
