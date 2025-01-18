#pragma once
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

class Cube {
   public:
    Cube(const glm::vec3& position);
    ~Cube();
    static inline const std::vector<float> getVertices() { return vertices; }
    inline glm::vec3 getPosition() const { return position; }

   private:
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    static std::vector<float> vertices;
};