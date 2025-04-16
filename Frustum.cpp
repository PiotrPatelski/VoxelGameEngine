#include "Frustum.hpp"

bool Frustum::isAABBInside(const glm::vec3 &min, const glm::vec3 &max) const {
    for (const auto &plane : planes) {
        // Compute the vertex in the AABB that is farthest along the plane
        // normal.
        glm::vec3 positive = min;
        if (plane.normal.x >= 0) positive.x = max.x;
        if (plane.normal.y >= 0) positive.y = max.y;
        if (plane.normal.z >= 0) positive.z = max.z;

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
    // Right plane: row4 - row1
    planes[0].normal.x = projView[0][3] - projView[0][0];
    planes[0].normal.y = projView[1][3] - projView[1][0];
    planes[0].normal.z = projView[2][3] - projView[2][0];
    planes[0].d = projView[3][3] - projView[3][0];
    normalizePlane(planes[0]);

    // Left plane: row4 + row1
    planes[1].normal.x = projView[0][3] + projView[0][0];
    planes[1].normal.y = projView[1][3] + projView[1][0];
    planes[1].normal.z = projView[2][3] + projView[2][0];
    planes[1].d = projView[3][3] + projView[3][0];
    normalizePlane(planes[1]);

    // Bottom plane: row4 + row2
    planes[2].normal.x = projView[0][3] + projView[0][1];
    planes[2].normal.y = projView[1][3] + projView[1][1];
    planes[2].normal.z = projView[2][3] + projView[2][1];
    planes[2].d = projView[3][3] + projView[3][1];
    normalizePlane(planes[2]);

    // Top plane: row4 - row2
    planes[3].normal.x = projView[0][3] - projView[0][1];
    planes[3].normal.y = projView[1][3] - projView[1][1];
    planes[3].normal.z = projView[2][3] - projView[2][1];
    planes[3].d = projView[3][3] - projView[3][1];
    normalizePlane(planes[3]);

    // Far plane: row4 - row3
    planes[4].normal.x = projView[0][3] - projView[0][2];
    planes[4].normal.y = projView[1][3] - projView[1][2];
    planes[4].normal.z = projView[2][3] - projView[2][2];
    planes[4].d = projView[3][3] - projView[3][2];
    normalizePlane(planes[4]);

    // Near plane: row4 + row3
    planes[5].normal.x = projView[0][3] + projView[0][2];
    planes[5].normal.y = projView[1][3] + projView[1][2];
    planes[5].normal.z = projView[2][3] + projView[2][2];
    planes[5].d = projView[3][3] + projView[3][2];
    normalizePlane(planes[5]);
}

void Frustum::normalizePlane(Plane &plane) {
    const auto length = glm::length(plane.normal);
    plane.normal /= length;
    plane.d /= length;
}