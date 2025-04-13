#pragma once
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

enum class CubeType { SAND, DIRT, GRASS, WATER, LOG, LEAVES };

class Cube {
   public:
    Cube(const glm::vec3& position, CubeType type);
    Cube(const Cube&) = delete;
    Cube(Cube&&) = delete;
    Cube& operator=(const Cube&) = delete;
    Cube& operator=(Cube&&) = delete;
    ~Cube();
    inline glm::vec3 getPosition() const { return position; }
    inline glm::mat4 getModel() const { return model; }
    inline CubeType getType() const { return currentType; }

   private:
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::mat4 model{1.0f};
    CubeType currentType{CubeType::GRASS};
};