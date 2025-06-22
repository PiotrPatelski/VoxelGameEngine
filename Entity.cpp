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

Entity::Entity()
    : entityPosition{67.0f, 30.0f, 55.0f},
      shader{std::make_unique<Shader>("shaders/entity_shader.vs",
                                      "shaders/entity_shader.fs")} {
    textureID = TextureManager::loadTextureFromFile("textures/steve-64x64.png");
    createLimbs();

    glGenBuffers(1, &elementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);
    hitbox = std::make_unique<Hitbox>(entityPosition, Steve::hitboxOffset,
                                      Steve::hitboxScale);
    lastTime = glfwGetTime();
}

Entity::~Entity() {
    if (elementBufferObject) {
        glDeleteBuffers(1, &elementBufferObject);
    }
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
    return glm::translate(glm::mat4(1.0f), entityPosition + Steve::bodyOffset) *
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
    const auto frontFacePosition = hitbox->getFrontFacePosition();
    const auto cubeTypeInFront = world.getCubeTypeAtPosition(frontFacePosition);
    const auto cubeTypeInFrontHeadLevel = world.getCubeTypeAtPosition(
        frontFacePosition + glm::vec3(0.0f, 1.0f, 0.0f));
    const auto cubeTypeBelow =
        world.getCubeTypeAtPosition(hitbox->getBottomFacePosition());
    if (cubeTypeBelow == CubeType::NONE) {
        isMoving = false;
        return;
    }
    if (cubeTypeInFrontHeadLevel != CubeType::NONE) {
        isMoving = false;
        return;
    }
    if (cubeTypeInFront != CubeType::NONE) {
        entityPosition += glm::vec3(0.0f, 1.75f, 0.0f);
    }
    isMoving = true;
    const auto forwardDirection = glm::normalize(
        glm::vec3(std::sin(directionAngle), 0.0f, std::cos(directionAngle)));
    entityPosition += forwardDirection * 0.05f;
    hitbox->setPosition(entityPosition);
}

void Entity::updateFallingMovement(const World& world) {
    const auto cubeTypeBelow =
        world.getCubeTypeAtPosition(hitbox->getBottomFacePosition());
    if (cubeTypeBelow == CubeType::NONE) {
        entityPosition.y -= 0.1f;
        hitbox->setPosition(entityPosition);
        isMoving = false;
    }
}

void Entity::update(const glm::mat4& view, const glm::mat4& projection,
                    const World& world) {
    updateFallingMovement(world);
    updateDirection();
    moveForward(world);
    updateShaders(view, projection);
    updateMoveAnimation();
}

void Entity::updateDirection() {
    float currentTime = glfwGetTime();
    bool hasTwoSecondsElapsed = (currentTime - lastTime) > 2.0f;
    if (hasTwoSecondsElapsed) {
        directionAngle = glm::radians(static_cast<float>(rand() % 360));
        lastTime = currentTime;
    }
}

void Entity::updateShaders(const glm::mat4& view, const glm::mat4& projection) {
    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setVec3("underwaterTint", glm::vec3(0.0f, 0.3f, 0.5f));
    shader->setFloat("underwaterMix", 0.5f);
    shader->setFloat("waterLevelY", 14.0f);
    hitbox->updateShaders(view, projection);
}

void Entity::updateMoveAnimation() {
    rotate();
    if (isMoving) {
        const auto speed = 2.0f;
        const auto maxAnimationAngle = glm::radians(30.0f);
        const auto currentAnimationAngle =
            std::sin(glfwGetTime() * speed) * maxAnimationAngle;
        const auto bodyRotation = glm::rotate(glm::mat4(1.0f), directionAngle,
                                              glm::vec3(0.0f, 1.0f, 0.0f));
        applyAnimationTransforms(currentAnimationAngle, bodyRotation);
    }
}

void Entity::applyAnimationTransforms(float currentAnimationAngle,
                                      const glm::mat4& bodyRotation) {
    headTransform =
        glm::translate(glm::mat4(1.0f), entityPosition + Steve::headOffset) *
        bodyRotation;

    bodyTransform =
        glm::translate(glm::mat4(1.0f), entityPosition + Steve::bodyOffset) *
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
    const auto bodyRotation = glm::rotate(glm::mat4(1.0f), directionAngle,
                                          glm::vec3(0.0f, 1.0f, 0.0f));
    const auto bodyOffsetTranslation =
        glm::translate(glm::mat4(1.0f), entityPosition + Steve::bodyOffset);

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
