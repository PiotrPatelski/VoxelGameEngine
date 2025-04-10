#pragma once
#include <array>
#include <glm/gtc/matrix_transform.hpp>

class Frustum {
   public:
    Frustum();
    Frustum(const Frustum &) = delete;
    Frustum(Frustum &&) = delete;
    Frustum &operator=(const Frustum &) = delete;
    Frustum &operator=(Frustum &&) = delete;
    // Each plane is defined as: normal.x*x + normal.y*y + normal.z*z + d = 0
    struct Plane {
        glm::vec3 normal;
        float d;
    };
    // Test an Axis-Aligned Bounding Box (AABB) against the
    //  frustum. For a cube of size 1 centered at a position, use min = pos -
    //  0.5, max = pos + 0.5.
    bool isAABBInside(const glm::vec3 &min, const glm::vec3 &max) const;
    void update(const glm::mat4 &projView);

   private:
    void normalizePlane(Plane &plane);
    std::array<Plane, 6> planes;
};