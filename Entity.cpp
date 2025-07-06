#include "Entity.hpp"
#include "VertexData.hpp"
#include "Camera.hpp"
#include "TextureManager.hpp"
#include "Hitbox.hpp"
#include "World.hpp"
#include "SteveData.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

Entity::Entity(const glm::vec3& initialPosition)
    : shader{std::make_unique<Shader>("shaders/entity_shader.vs",
                                      "shaders/entity_shader.fs")} {
    textureID = TextureManager::loadTextureFromFile("textures/steve-64x64.png");
    createLimbs();

    glGenBuffers(1, &elementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);
    hitbox = std::make_shared<Hitbox>(initialPosition, Steve::hitboxOffset,
                                      Steve::hitboxScale);
    physicsComponent =
        std::make_unique<PhysicsComponent>(initialPosition, hitbox);
    lastTime = glfwGetTime();
}

Entity::~Entity() {
    if (elementBufferObject) {
        glDeleteBuffers(1, &elementBufferObject);
    }
}

void Entity::setPosition(const glm::vec3& position) {
    physicsComponent->setPosition(position);
}

void Entity::createLimbs() {
    head = std::make_unique<BodyPart>(Steve::headTexCoords, Steve::headOffset,
                                      Steve::headScale, textureID);
    body = std::make_unique<BodyPart>(Steve::bodyTexCoords, Steve::bodyOffset,
                                      Steve::bodyScale, textureID);
    leftArm = std::make_unique<BodyPart>(Steve::leftArmTexCoords,
                                         Steve::leftArmOffset, Steve::armScale,
                                         textureID);
    rightArm = std::make_unique<BodyPart>(Steve::rightArmTexCoords,
                                          Steve::rightArmOffset,
                                          Steve::armScale, textureID);
    leftLeg = std::make_unique<BodyPart>(Steve::leftLegTexCoords,
                                         Steve::leftLegOffset, Steve::legScale,
                                         textureID);
    rightLeg = std::make_unique<BodyPart>(Steve::rightLegTexCoords,
                                          Steve::rightLegOffset,
                                          Steve::legScale, textureID);
}

glm::mat4 Entity::createLimbTransform(const glm::vec3& limbOffset,
                                      float animationAngle,
                                      const glm::mat4& bodyRotation) const {
    const auto currentPosition = physicsComponent->getPosition();
    return glm::translate(glm::mat4(1.0f),
                          currentPosition + Steve::bodyOffset) *
           bodyRotation *
           glm::translate(glm::mat4(1.0f),
                          limbOffset - Steve::bodyOffset +
                              Steve::centerPointToTopRotationAxisOffset) *
           glm::rotate(glm::mat4(1.0f), animationAngle,
                       glm::vec3(1.0f, 0.0f, 0.0f)) *
           glm::translate(glm::mat4(1.0f),
                          -Steve::centerPointToTopRotationAxisOffset);
}

void Entity::moveForward(const World& world) {
    const bool moved = physicsComponent->moveForward(
        world, physicsComponent->getDirectionAngle());

    hitbox->setPosition(physicsComponent->getPosition());
    isMoving = moved;
}

void Entity::updateFallingMovement(const World& world) {
    const bool fell = physicsComponent->updateFalling(world);

    hitbox->setPosition(physicsComponent->getPosition());
    if (fell) {
        isMoving = false;
    }
}

void Entity::updatePhysics(const World& world) {
    updateFallingMovement(world);
    updateDirection();
    moveForward(world);
}

void Entity::updateRendering(const Camera& camera) {
    updateShaders(camera);
    updateMoveAnimation();
}

void Entity::updateDirection() {
    float currentTime = glfwGetTime();
    bool hasTwoSecondsElapsed = (currentTime - lastTime) > 2.0f;
    if (hasTwoSecondsElapsed) {
        physicsComponent->setDirectionAngle(
            glm::radians(static_cast<float>(rand() % 360)));
        lastTime = currentTime;
    }
}

void Entity::updateShaders(const Camera& camera) {
    const auto view = camera.getViewMatrix();
    const auto projection = camera.getProjectionMatrix();
    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setVec3("underwaterTint", glm::vec3(0.0f, 0.3f, 0.5f));
    shader->setFloat("underwaterMix", camera.isUnderwater() ? 0.5f : 0.0f);
    shader->setBool("isUnderwater", camera.isUnderwater());
    hitbox->updateShaders(view, projection);
}

void Entity::updateMoveAnimation() {
    rotate();
    if (isMoving) {
        const auto speed = 2.0f;
        const auto maxAnimationAngle = glm::radians(30.0f);
        const auto currentAnimationAngle =
            std::sin(glfwGetTime() * speed) * maxAnimationAngle;
        const auto bodyRotation =
            glm::rotate(glm::mat4(1.0f), physicsComponent->getDirectionAngle(),
                        glm::vec3(0.0f, 1.0f, 0.0f));
        applyAnimationTransforms(currentAnimationAngle, bodyRotation);
    }
}

void Entity::applyAnimationTransforms(float currentAnimationAngle,
                                      const glm::mat4& bodyRotation) {
    const auto currentPosition = physicsComponent->getPosition();
    headTransform =
        glm::translate(glm::mat4(1.0f), currentPosition + Steve::headOffset) *
        bodyRotation;

    bodyTransform =
        glm::translate(glm::mat4(1.0f), currentPosition + Steve::bodyOffset) *
        bodyRotation;

    leftArmTransform = createLimbTransform(Steve::leftArmOffset,
                                           currentAnimationAngle, bodyRotation);
    rightArmTransform = createLimbTransform(
        Steve::rightArmOffset, -currentAnimationAngle, bodyRotation);

    leftLegTransform = createLimbTransform(
        Steve::leftLegOffset, -currentAnimationAngle, bodyRotation);
    rightLegTransform = createLimbTransform(
        Steve::rightLegOffset, currentAnimationAngle, bodyRotation);
}

void Entity::rotate() {
    const auto bodyRotation =
        glm::rotate(glm::mat4(1.0f), physicsComponent->getDirectionAngle(),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    const auto currentPosition = physicsComponent->getPosition();
    const auto bodyOffsetTranslation =
        glm::translate(glm::mat4(1.0f), currentPosition + Steve::bodyOffset);

    applyRotationTransforms(bodyRotation, bodyOffsetTranslation);

    hitbox->setRotation(bodyRotation);
}

void Entity::applyRotationTransforms(const glm::mat4& bodyRotation,
                                     const glm::mat4& bodyOffsetTranslation) {
    headTransform =
        bodyOffsetTranslation * bodyRotation *
        glm::translate(glm::mat4(1.0f), Steve::headOffset - Steve::bodyOffset);

    bodyTransform = bodyOffsetTranslation * bodyRotation;

    leftArmTransform = bodyOffsetTranslation * bodyRotation *
                       glm::translate(glm::mat4(1.0f),
                                      Steve::leftArmOffset - Steve::bodyOffset);

    rightArmTransform = bodyOffsetTranslation * bodyRotation *
                        glm::translate(glm::mat4(1.0f), Steve::rightArmOffset -
                                                            Steve::bodyOffset);

    leftLegTransform = bodyOffsetTranslation * bodyRotation *
                       glm::translate(glm::mat4(1.0f),
                                      Steve::leftLegOffset - Steve::bodyOffset);

    rightLegTransform = bodyOffsetTranslation * bodyRotation *
                        glm::translate(glm::mat4(1.0f), Steve::rightLegOffset -
                                                            Steve::bodyOffset);
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
