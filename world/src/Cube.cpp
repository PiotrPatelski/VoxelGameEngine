#include "Cube.hpp"

Cube::Cube(const glm::vec3& initialPosition, CubeType type)
    : position{initialPosition},
      model{glm::translate(glm::mat4{1.0f}, initialPosition)},
      currentType{type} {}

Cube::~Cube() {}