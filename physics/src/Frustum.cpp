#include "Frustum.hpp"

bool Frustum::isAABBInside(const glm::vec3 &min, const glm::vec3 &max) const {
    for (const auto &plane : planes) {
        // Compute the vertex in the AABB that is farthest along the plane
        // normal.
        glm::vec3 positive = min;
        if (plane.normal.x >= 0) {
            positive.x = max.x;
        }
        if (plane.normal.y >= 0) {
            positive.y = max.y;
        }
        if (plane.normal.z >= 0) {
            positive.z = max.z;
        }
        // If this vertex is outside, then the entire AABB is outside.
        if (glm::dot(plane.normal, positive) + plane.d < 0) {
            return false;
        }
    }
    return true;
}

bool Frustum::isModelIncluded(const glm::mat4 &cubeModel) const {
    const auto pos = glm::vec3(cubeModel[3]);
    const auto voxelMin = pos - glm::vec3(0.5f + cubeToleranceOutsideBounds);
    const auto voxelMax = pos + glm::vec3(0.5f + cubeToleranceOutsideBounds);
    return isAABBInside(voxelMin, voxelMax);
}

Frustum::Frustum() : cubeToleranceOutsideBounds{7.f} {}

void Frustum::update(const glm::mat4 &projView) {
    // GLM is column‑major: projView[c][r]
    // Build row vectors (r0…r3)
    const glm::vec4 r0(projView[0][0], projView[1][0], projView[2][0],
                       projView[3][0]);
    const glm::vec4 r1(projView[0][1], projView[1][1], projView[2][1],
                       projView[3][1]);
    const glm::vec4 r2(projView[0][2], projView[1][2], projView[2][2],
                       projView[3][2]);
    const glm::vec4 r3(projView[0][3], projView[1][3], projView[2][3],
                       projView[3][3]);

    // left plane   = r3 + r0
    planes[0].normal.x = r3.x + r0.x;
    planes[0].normal.y = r3.y + r0.y;
    planes[0].normal.z = r3.z + r0.z;
    planes[0].d = r3.w + r0.w;

    // right plane  = r3 - r0
    planes[1].normal.x = r3.x - r0.x;
    planes[1].normal.y = r3.y - r0.y;
    planes[1].normal.z = r3.z - r0.z;
    planes[1].d = r3.w - r0.w;

    // bottom plane = r3 + r1
    planes[2].normal.x = r3.x + r1.x;
    planes[2].normal.y = r3.y + r1.y;
    planes[2].normal.z = r3.z + r1.z;
    planes[2].d = r3.w + r1.w;

    // top plane    = r3 - r1
    planes[3].normal.x = r3.x - r1.x;
    planes[3].normal.y = r3.y - r1.y;
    planes[3].normal.z = r3.z - r1.z;
    planes[3].d = r3.w - r1.w;

    // far plane    = r3 - r2
    planes[4].normal.x = r3.x - r2.x;
    planes[4].normal.y = r3.y - r2.y;
    planes[4].normal.z = r3.z - r2.z;
    planes[4].d = r3.w - r2.w;

    // near plane   = r3 + r2
    planes[5].normal.x = r3.x + r2.x;
    planes[5].normal.y = r3.y + r2.y;
    planes[5].normal.z = r3.z + r2.z;
    planes[5].d = r3.w + r2.w;

    // normalize all planes
    for (int i = 0; i < 6; ++i) {
        normalizePlane(planes[i]);
    }
}

void Frustum::normalizePlane(Plane &plane) {
    const auto length = glm::length(plane.normal);
    plane.normal /= length;
    plane.d /= length;
}