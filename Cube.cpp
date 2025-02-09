#include "Cube.hpp"

Cube::Cube(const glm::vec3& initialPosition)
    : position{initialPosition},
      model{glm::translate(glm::mat4{1.0f}, initialPosition)} {}

Cube::~Cube() {}