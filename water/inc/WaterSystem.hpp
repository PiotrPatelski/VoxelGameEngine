#pragma once
#include "Cube.hpp"
#include <glm/vec3.hpp>

namespace WaterSystem {

inline bool isWater(CubeType type) {
    return type == CubeType::WATER_SOURCE or type == CubeType::WATER_FLOWING;
}

inline bool isWaterSource(CubeType type) {
    return type == CubeType::WATER_SOURCE;
}

inline float getWaterHeight(CubeType type) {
    if (type == CubeType::WATER_SOURCE) {
        return 1.0f;
    }
    if (type == CubeType::WATER_FLOWING) {
        return 0.8f;
    }
    return 0.0f;
}

inline bool shouldRenderWaterFace(CubeType current, CubeType neighbor) {
    if (isWater(current) and isWater(neighbor)) {
        return getWaterHeight(neighbor) < getWaterHeight(current);
    }
    return not isWater(neighbor);
}

} // namespace WaterSystem
