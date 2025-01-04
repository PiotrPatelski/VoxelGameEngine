#include "Camera.hpp"

Camera::Camera(const glm::vec3& position, const glm::vec3& up, float yaw,
               float pitch)
    : position(position),
      worldUpVec(up),
      frontVec(glm::vec3(0.0f, 0.0f, -1.0f)),
      movementSpeed(SPEED),
      mouseSensitivity(SENSITIVITY),
      yaw{yaw},
      pitch{pitch},
      zoom{ZOOM} {
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY,
               float upZ, float yaw, float pitch)
    : position(glm::vec3(posX, posY, posZ)),
      worldUpVec(glm::vec3(upX, upY, upZ)),
      frontVec(glm::vec3(0.0f, 0.0f, -1.0f)),
      movementSpeed(SPEED),
      mouseSensitivity(SENSITIVITY),
      yaw{yaw},
      pitch{pitch},
      zoom{ZOOM} {
    updateCameraVectors();
}

// processes input received from any keyboard-like input system. Accepts input
// parameter in the form of camera defined ENUM (to abstract it from windowing
// systems)
void Camera::processKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    if (direction == FORWARD) position += frontVec * velocity;
    if (direction == BACKWARD) position -= frontVec * velocity;
    if (direction == LEFT) position -= rightVec * velocity;
    if (direction == RIGHT) position += rightVec * velocity;
}

// processes input received from a mouse input system. Expects the offset value
// in both the x and y direction.
void Camera::processMouseMovement(float xoffset, float yoffset,
                                  GLboolean constrainPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }

    // update front, rightVec and upVec Vectors using the updated Euler angles
    updateCameraVectors();
}

// processes input received from a mouse scroll-wheel event. Only requires input
// on the vertical wheel-axis
void Camera::processMouseScroll(float yoffset) {
    zoom -= (float)yoffset;
    if (zoom < 1.0f) zoom = 1.0f;
    if (zoom > 45.0f) zoom = 45.0f;
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors() {
    // calculate the new front vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    frontVec = glm::normalize(front);
    // also re-calculate the rightVec and upVec vector
    rightVec = glm::normalize(glm::cross(
        front, worldUpVec)); // normalize the vectors, because their length gets
                             // closer to 0 the more you look up or down which
                             // results in slower movement.
    upVec = glm::normalize(glm::cross(rightVec, frontVec));
}
