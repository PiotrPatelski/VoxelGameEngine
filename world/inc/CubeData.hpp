#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>
#include "Cube.hpp"

/** Small data struct passed between the background thread and the main thread.
 */
struct CubeData {
    std::vector<std::unique_ptr<Cube>> cubes{};
    std::unordered_map<CubeType, std::vector<glm::mat4>>
        instanceModelMatrices{};
    std::vector<float> lightVolume{};
};