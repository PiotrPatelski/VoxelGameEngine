#include "Cube.hpp"

Cube::Cube(const glm::vec3& position)
    : position{position}, model{glm::translate(glm::mat4{1.0f}, position)} {}

Cube::~Cube() {}