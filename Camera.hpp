

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to
// stay away from window-system specific input methods
enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// An abstract camera class that processes input and calculates the
// corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera {
   public:
    Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f),
           const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW,
           float pitch = PITCH);

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ,
           float yaw, float pitch);

    // returns the view matrix calculated using Euler Angles and the LookAt
    // Matrix
    inline glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + frontVec, upVec);
    }
    inline float getZoom() const { return zoom; }

    void processKeyboard(Camera_Movement direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset,
                              GLboolean constrainPitch = true);
    void processMouseScroll(float yoffset);

   private:
    void updateCameraVectors();

    // camera Attributes
    glm::vec3 position;
    glm::vec3 frontVec;
    glm::vec3 upVec;
    glm::vec3 rightVec;
    glm::vec3 worldUpVec;
    // euler Angles
    float yaw;
    float pitch;
    // camera options
    float movementSpeed;
    float mouseSensitivity;
    float zoom;
};
