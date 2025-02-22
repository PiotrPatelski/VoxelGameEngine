

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to
// stay away from window-system specific input methods
enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

// An abstract camera class that processes input and calculates the
// corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera {
   public:
    Camera();

    // returns the view matrix calculated using Euler Angles and the LookAt
    // Matrix
    inline glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + frontVec, upVec);
    }
    inline float getZoom() const { return zoom; }
    inline glm::vec3 getPosition() const { return position; }
    inline glm::vec3 getFront() const { return frontVec; }

    void processKeyboard(Camera_Movement direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset,
                              GLboolean constrainPitch = true);
    void processMouseScroll(float yoffset);

   private:
    void updateCameraVectors();

    // camera Attributes
    glm::vec3 position{64.0f, 30.0f, 64.0f};
    glm::vec3 frontVec{0.0f, 0.0f, -1.0f};
    glm::vec3 upVec{0.0f, 0.0f, 0.0f};
    glm::vec3 rightVec{0.0f, 0.0f, 0.0f};
    glm::vec3 worldUpVec{0.0f, 1.0f, 0.0f};
    // euler Angles
    float yaw{-90.0f};
    float pitch{0.0f};
    // camera options
    float movementSpeed{2.5f};
    float mouseSensitivity{0.1f};
    float zoom{45.0f};
};
