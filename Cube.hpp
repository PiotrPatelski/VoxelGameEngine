#pragma once
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.hpp"

class Cube {
   public:
    Cube(const glm::vec3& position);
    ~Cube();
    inline glm::vec3 getPosition() const { return position; }
    inline glm::mat4 getModel() const { return model; }

   private:
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::mat4 model{1.0f};
};