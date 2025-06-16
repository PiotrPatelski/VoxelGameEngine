#include "Camera.hpp"

Camera::Camera(unsigned int width, unsigned int height)
    : screenWidth{static_cast<float>(width)},
      screenHeight{static_cast<float>(height)} {
    updateCameraVectors();
    calculateProjection();
}

// processes input received from any keyboard-like input system. Accepts input
// parameter in the form of camera defined ENUM (to abstract it from windowing
// systems)
void Camera::processKeyboard(Camera_Movement direction, float deltaTime) {
    const auto velocity = movementSpeed * deltaTime;
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
    zoom -= yoffset;
    if (zoom < 1.0f) {
        zoom = 1.0f;
    }
    if (zoom > 45.0f) {
        zoom = 45.0f;
    }
    calculateProjection();
}

void Camera::calculateProjection() {
    projectionMatrix = glm::perspective(
        glm::radians(zoom), screenWidth / screenHeight, 0.1f, 200.0f);
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors() {
    // calculate the new front vector
    glm::vec3 front{};
    front.x =
        static_cast<float>(cos(glm::radians(yaw)) * cos(glm::radians(pitch)));
    front.y = static_cast<float>(sin(glm::radians(pitch)));
    front.z =
        static_cast<float>(sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
    frontVec = glm::normalize(front);
    // also re-calculate the rightVec and upVec vector
    rightVec = glm::normalize(glm::cross(
        front, worldUpVec)); // normalize the vectors, because their length gets
                             // closer to 0 the more you look up or down which
                             // results in slower movement.
    upVec = glm::normalize(glm::cross(rightVec, frontVec));
}
