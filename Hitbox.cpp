#include "Hitbox.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "VertexData.hpp"

Hitbox::Hitbox(const glm::vec3& hitboxPosition, const glm::vec3& hitboxOffset,
               const glm::vec3& hitboxScale)
    : position{hitboxPosition},
      offset{hitboxOffset},
      scale{hitboxScale},
      shader{std::make_unique<Shader>("shaders/hitbox_shader.vs",
                                      "shaders/hitbox_shader.fs")} {
    setupBuffers();
}

Hitbox::~Hitbox() {
    if (elementBufferObject) {
        glDeleteBuffers(1, &elementBufferObject);
    }
    if (vertexArrayObject) {
        glDeleteVertexArrays(1, &vertexArrayObject);
    }
    if (vertexBufferObject) {
        glDeleteBuffers(1, &vertexBufferObject);
    }
}

void Hitbox::setupBuffers() {
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &elementBufferObject);

    glBindVertexArray(vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(glm::vec3),
                 cubeVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 hitboxIndices.size() * sizeof(unsigned int),
                 hitboxIndices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Hitbox::updateShaders(const glm::mat4& view, const glm::mat4& projection) {
    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
}

void Hitbox::setRotation(const glm::mat4& rotationMatrix) {
    rotation = rotationMatrix;
}

void Hitbox::render() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    shader->use();
    glm::mat4 model =
        glm::translate(glm::mat4(1.0f), position + offset) * rotation;
    model = glm::scale(model, scale);
    shader->setMat4("model", model);
    glBindVertexArray(vertexArrayObject);
    glDrawElements(GL_LINES, hitboxIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glUseProgram(0);
}

glm::vec3 Hitbox::getBottomFacePosition() const {
    return position + offset - glm::vec3(0.0f, (scale.y / 2.0f) - 0.375f, 0.0f);
}

void Hitbox::setPosition(const glm::vec3& newPosition) {
    position = newPosition;
}

glm::vec3 Hitbox::getFrontFacePosition() const {
    return position + offset + glm::vec3(0.0f, 0.0f, (scale.z / 2.0f) + 0.6f);
}
