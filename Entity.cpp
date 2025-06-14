#include "Entity.hpp"
#include "VertexData.hpp"
#include "Camera.hpp"
#include "TextureManager.hpp"
#include "Hitbox.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

namespace {
const glm::vec3 headScale{0.5f, 0.5f, 0.5f};
const glm::vec3 bodyScale{0.5f, 0.75f, 0.25f};
const glm::vec3 armScale{0.25f, 0.75f, 0.25f};
const glm::vec3 legScale{0.25f, 0.75f, 0.25f};
const glm::vec3 headOffset{0.0f, 1.625f, 0.0f};
const glm::vec3 bodyOffset{0.0f, 1.0f, 0.0f};
const glm::vec3 leftArmOffset{-0.375f, 1.0f, 0.0f};
const glm::vec3 rightArmOffset{0.375f, 1.0f, 0.0f};
const glm::vec3 leftLegOffset{-0.125f, 0.25f, 0.0f};
const glm::vec3 rightLegOffset{0.125f, 0.25f, 0.0f};
const glm::vec3 hitboxOffset{0.f, 0.875f, 0.f};
const glm::vec3 hitboxScale{1.f, 2.0f, 0.5f};
const glm::vec3 centerPointToTopRotationAxisOffset{0.0f, 0.5f, 0.0f};
const glm::vec3 topPointToCenterRotationAxisOffset{0.0f, -0.5f, 0.0f};
} // namespace

Entity::Entity()
    : entityPosition{64.0f, 30.0f, 64.0f},
      shader{std::make_unique<Shader>("shaders/entity_shader.vs",
                                      "shaders/entity_shader.fs")} {
    textureID = TextureManager::loadTextureFromFile("textures/steve-64x64.png");

    head = std::make_unique<BodyPart>(headTexCoords, headOffset, headScale,
                                      textureID);
    body = std::make_unique<BodyPart>(bodyTexCoords, bodyOffset, bodyScale,
                                      textureID);
    leftArm = std::make_unique<BodyPart>(leftArmTexCoords, leftArmOffset,
                                         armScale, textureID);
    rightArm = std::make_unique<BodyPart>(rightArmTexCoords, rightArmOffset,
                                          armScale, textureID);
    leftLeg = std::make_unique<BodyPart>(leftLegTexCoords, leftLegOffset,
                                         legScale, textureID);
    rightLeg = std::make_unique<BodyPart>(rightLegTexCoords, rightLegOffset,
                                          legScale, textureID);
    glGenBuffers(1, &elementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);
    hitbox =
        std::make_unique<Hitbox>(entityPosition, hitboxOffset, hitboxScale);
}

Entity::~Entity() {
    if (elementBufferObject) {
        glDeleteBuffers(1, &elementBufferObject);
    }
}

glm::mat4 Entity::createLimbTransform(const glm::vec3& offset, float angle,
                                      const glm::vec3& rotationAxis) const {
    glm::mat4 transform =
        glm::translate(glm::mat4(1.0f), entityPosition + offset);
    transform = glm::translate(transform, centerPointToTopRotationAxisOffset);
    transform = glm::rotate(transform, angle, rotationAxis);
    transform = glm::translate(transform, topPointToCenterRotationAxisOffset);
    return transform;
}

void Entity::update(const glm::mat4& view, const glm::mat4& projection) {
    updateShaders(view, projection);
    updateMoveAnimation();
}

void Entity::updateShaders(const glm::mat4& view, const glm::mat4& projection) {
    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    hitbox->updateShaders(view, projection);
}

void Entity::updateMoveAnimation() {
    const auto speed = 2.0f;
    const auto maxAngle = glm::radians(30.0f);
    const auto angle = std::sin(glfwGetTime() * speed) * maxAngle;
    const auto rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
    headTransform =
        glm::translate(glm::mat4(1.0f), entityPosition + headOffset);
    bodyTransform =
        glm::translate(glm::mat4(1.0f), entityPosition + bodyOffset);

    leftArmTransform = createLimbTransform(leftArmOffset, angle, rotationAxis);
    rightArmTransform =
        createLimbTransform(rightArmOffset, -angle, rotationAxis);

    leftLegTransform = createLimbTransform(leftLegOffset, -angle, rotationAxis);
    rightLegTransform =
        createLimbTransform(rightLegOffset, angle, rotationAxis);
}

void Entity::render() {
    head->render(headTransform, *shader, elementBufferObject);
    body->render(bodyTransform, *shader, elementBufferObject);
    leftArm->render(leftArmTransform, *shader, elementBufferObject);
    rightArm->render(rightArmTransform, *shader, elementBufferObject);
    leftLeg->render(leftLegTransform, *shader, elementBufferObject);
    rightLeg->render(rightLegTransform, *shader, elementBufferObject);
    hitbox->render();
}
