#pragma once
#include <array>
#include <glm/gtc/matrix_transform.hpp>

class Frustum {
   public:
    Frustum();
    // Each plane is defined as: normal.x*x + normal.y*y + normal.z*z + d = 0
    struct Plane {
        glm::vec3 normal;
        float d;
    };

    std::array<Plane, 6> planes;

    // Update the frustum planes from the combined projection * view matrix.
    void update(const glm::mat4 &projView);

   private:
    void normalizePlane(Plane &plane);
};

// A helper function to test an Axis-Aligned Bounding Box (AABB) against the
// frustum. For a cube of size 1 centered at a position, we use min = pos - 0.5,
// max = pos + 0.5.
bool isAABBInsideFrustum(const glm::vec3 &min, const glm::vec3 &max,
                         const Frustum &frustum);