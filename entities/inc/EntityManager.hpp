#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <glm/glm.hpp>
#include "Entity.hpp"
#include "Camera.hpp"
#include "World.hpp"
#include "ThreadPool.hpp"

using EntityID = uint32_t;

class EntityManager {
   public:
    EntityManager();
    ~EntityManager();
    EntityManager(const EntityManager&) = delete;
    EntityManager(EntityManager&&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;
    EntityManager& operator=(EntityManager&&) = delete;
    EntityID createEntity(const glm::vec3& position);

    void update(const World& world, const Camera& camera);
    void render(const Camera& camera);

   private:
    std::unordered_map<EntityID, std::unique_ptr<Entity>> entities;
    EntityID nextEntityID{1};

    std::unique_ptr<ThreadPool> physicsPool{nullptr};
    std::mutex entitiesMutex;

    void updatePhysicsAsync(const World& world, const Camera& camera);
    void updateRenderingSync(const Camera& camera);

    bool isEntityInRenderRange(const Entity& entity,
                               const Camera& camera) const;
    bool isEntityInUpdateRange(const Entity& entity,
                               const Camera& camera) const;
    float getDistanceToCamera(const Entity& entity, const Camera& camera) const;
};
