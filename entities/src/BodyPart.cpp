#include "BodyPart.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "VertexDataBuilder.hpp"

BodyPart::BodyPart(const std::vector<glm::vec2>& texCoords,
                   const glm::vec3& modelOffset, const glm::vec3& modelScale,
                   GLuint texture)
    : vertices{createEntityVertexVector(texCoords)},
      offset(modelOffset),
      scale(modelScale),
      textureID(texture) {
    initVertexAttributes();
}

BodyPart::~BodyPart() {
    if (vertexBufferObject) {
        glDeleteBuffers(1, &vertexBufferObject);
    }
    if (vertexArrayObject) {
        glDeleteVertexArrays(1, &vertexArrayObject);
    }
}

void BodyPart::initVertexAttributes() {
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);

    glBindVertexArray(vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(EntityVertex),
                 vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(EntityVertex),
                          (void*)offsetof(EntityVertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(EntityVertex),
                          (void*)offsetof(EntityVertex, texCoord));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void BodyPart::render(const glm::mat4& transform, Shader& shader,
                      int elementBufferObject) {
    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    shader.setInt("entityTexture", 0);

    const auto model = glm::scale(transform, scale);
    shader.setMat4("model", model);

    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
