#include "EntityManager.hpp"
#include <cmath>
#include <iostream>
#include <thread>

namespace {
static constexpr float DEFAULT_RENDER_DISTANCE{100.0f};
static constexpr float DEFAULT_UPDATE_DISTANCE{150.0f};
} // namespace

EntityManager::EntityManager()
    : physicsPool(std::make_unique<ThreadPool>(
          std::thread::hardware_concurrency() - 1)) {
    std::cout << "EntityManager::Init!" << std::endl;
}

EntityManager::~EntityManager() {
    std::cout << "EntityManager::Shutdown!" << std::endl;
}

EntityID EntityManager::createEntity(const glm::vec3& position) {
    EntityID id{nextEntityID++};

    entities[id] = std::make_unique<Entity>(position);

    return id;
}

void EntityManager::update(const World& world, const Camera& camera) {
    updatePhysicsAsync(world, camera);
    updateRenderingSync(camera);
}

void EntityManager::updatePhysicsAsync(const World& world,
                                       const Camera& camera) {
    std::lock_guard<std::mutex> lock(entitiesMutex);
    for (auto& [_, entity] : entities) {
        if (isEntityInUpdateRange(*entity, camera)) {
            physicsPool->enqueue(
                [&entity, &world]() { entity->updatePhysics(world); });
        }
    }

    physicsPool->waitForAll();
}

void EntityManager::updateRenderingSync(const Camera& camera) {
    std::lock_guard<std::mutex> lock(entitiesMutex);
    for (auto& [_, entity] : entities) {
        if (isEntityInUpdateRange(*entity, camera)) {
            entity->updateRendering(camera.getViewMatrix(),
                                    camera.getProjectionMatrix());
        }
    }
}

void EntityManager::render(const Camera& camera) {
    std::lock_guard<std::mutex> lock(entitiesMutex);
    for (auto& [_, entity] : entities) {
        if (isEntityInRenderRange(*entity, camera)) {
            entity->render();
        }
    }
}

bool EntityManager::isEntityInRenderRange(const Entity& entity,
                                          const Camera& camera) const {
    return getDistanceToCamera(entity, camera) <= DEFAULT_RENDER_DISTANCE;
}

bool EntityManager::isEntityInUpdateRange(const Entity& entity,
                                          const Camera& camera) const {
    return getDistanceToCamera(entity, camera) <= DEFAULT_UPDATE_DISTANCE;
}

float EntityManager::getDistanceToCamera(const Entity& entity,
                                         const Camera& camera) const {
    const auto entityPos = entity.getPosition();
    const auto cameraPos = camera.getPosition();
    const auto diff = entityPos - cameraPos;
    return std::sqrt(glm::dot(diff, diff));
}
