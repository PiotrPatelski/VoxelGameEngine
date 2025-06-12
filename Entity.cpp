#include "Entity.hpp"
#include "VertexData.hpp"
#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace {
const glm::vec3 headScale{0.5f, 0.5f, 0.5f};
const glm::vec3 bodyScale{0.5f, 0.75f, 0.25f};
const glm::vec3 armScale{0.25f, 0.75f, 0.25f};
const glm::vec3 legScale{0.25f, 0.75f, 0.25f};
const glm::vec3 headOffset{0.0f, 1.6f, 0.0f};
const glm::vec3 bodyOffset{0.0f, 1.0f, 0.0f};
const glm::vec3 leftArmOffset{-0.375f, 1.0f, 0.0f};
const glm::vec3 rightArmOffset{0.375f, 1.0f, 0.0f};
const glm::vec3 leftLegOffset{-0.125f, 0.375f, 0.0f};
const glm::vec3 rightLegOffset{0.125f, 0.375f, 0.0f};
} // namespace

Entity::Entity()
    : position{64.0f, 30.0f, 64.0f},
      shader{std::make_unique<Shader>("shaders/entity_shader.vs",
                                      "shaders/entity_shader.fs")} {
    for (const auto& vertex : vertices) {
        entityVertices.push_back(vertex.position);
    }
    initVertexAttributes();
}

Entity::~Entity() {
    if (vertexBufferObject) {
        glDeleteBuffers(1, &vertexBufferObject);
    }
    if (elementBufferObject) {
        glDeleteBuffers(1, &elementBufferObject);
    }
    if (vertexArrayObject) {
        glDeleteVertexArrays(1, &vertexArrayObject);
    }
}

void Entity::initVertexAttributes() {
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &elementBufferObject);

    glBindVertexArray(vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, entityVertices.size() * sizeof(glm::vec3),
                 entityVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                          (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Entity::update(const glm::mat4& view, const glm::mat4& projection) {
    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
}

void Entity::renderCube(const glm::vec3& pos, const glm::vec3& scale) {
    const auto translatedMatrix = glm::translate(glm::mat4(1.0f), pos);
    const auto scaledMatrix = glm::scale(translatedMatrix, scale);
    shader->setMat4("model", scaledMatrix);
    glBindVertexArray(vertexArrayObject);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Entity::render() {
    shader->use();

    renderCube(position + headOffset, headScale);
    renderCube(position + bodyOffset, bodyScale);
    renderCube(position + leftArmOffset, armScale);
    renderCube(position + rightArmOffset, armScale);
    renderCube(position + leftLegOffset, legScale);
    renderCube(position + rightLegOffset, legScale);
}
