#include "World.hpp"

// BUFFER DEFAULTS
unsigned int vertexBufferObjects,
    elementBufferObjects; // must be overwritten before use
// unsigned int lightSourceVAO;
const unsigned int amountOfVBOBuffers{1};
const unsigned int amountOfEBOBuffers{1};

void setupVertexBufferData() {
    // FIRST CUBES
    //  set up vertex data (and buffer(s)) and configure vertex attributes
    //  ------------------------------------------------------------------
    glGenBuffers(amountOfVBOBuffers, &vertexBufferObjects);
    // glGenVertexArrays(amountOfVAOBuffers, &vertexArrayObjects);
    glGenBuffers(amountOfEBOBuffers, &elementBufferObjects);

    // glBindVertexArray(vertexArrayObjects);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects);
    glBufferData(GL_ARRAY_BUFFER, Chunk::getVertices().size() * sizeof(Vertex),
                 Chunk::getVertices().data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObjects);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 Chunk::getIndices().size() * sizeof(unsigned int),
                 Chunk::getIndices().data(), GL_STATIC_DRAW);
    // glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean
    // normalized, GLsizei stride, const void *pointer);
    // const unsigned int stride = sizeof(Vertex);
    // // POSITION
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    // glEnableVertexAttribArray(0);
    // // COLOR
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
    //                       (void*)offsetof(Vertex, color));
    // glEnableVertexAttribArray(1);
    // // TEXCOORDS
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
    //                       (void*)offsetof(Vertex, texCoord));
    // glEnableVertexAttribArray(2);
    // // NORMAL
    // glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride,
    //                       (void*)offsetof(Vertex, normal));
    // glEnableVertexAttribArray(3);

    // glBindVertexArray(0);

    // SECOND LIGHT
    // {    glGenVertexArrays(amountOfVAOBuffers, &lightSourceVAO);
    //     glBindVertexArray(lightSourceVAO);

    //     glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects);
    //     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    //     glEnableVertexAttribArray(0);
    //     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
    //                           (void*)(3 * sizeof(float)));
    //     glEnableVertexAttribArray(1);
    //     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
    //                           (void*)(6 * sizeof(float)));
    //     glEnableVertexAttribArray(2);}

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //DRAW LINES ONLY - FOR
    // DEBUG glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

World::World() {
    printf("World::Init!\n");
    setupVertexBufferData();

    for (int x{0}; x < worldSize; x++) {
        chunks.emplace_back(std::vector<std::unique_ptr<Chunk>>{});
        for (int z{0}; z < worldSize; z++) {
            chunks[x].emplace_back(std::make_unique<Chunk>(
                x, z, vertexBufferObjects, elementBufferObjects));
        }
    }
}

void World::render(Shader& shader) {
    // glBindVertexArray(vertexArrayObjects);
    shader.use();
    for (auto& x : chunks) {
        for (auto& z : x) {
            // z->applyCubeModels();
            // printf("Render Chunk\n");
            z->render(shader);
        }
    }
    // glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0,
    //                         modelMatrices->size());

    // lightCubeShader->use();
    // glBindVertexArray(lightSourceVAO);
    // for (const auto& position : pointLightPositions) {
    //     auto lightModel = glm::mat4(1.0f);
    //     lightModel = glm::translate(lightModel, position);
    //     lightModel = glm::scale(lightModel, glm::vec3(0.2f));
    //     lightCubeShader->setMat4("model", lightModel);
    //     glDrawArrays(GL_TRIANGLES, 0, 36);
    // }
    // glBindVertexArray(0);
}

World::~World() {
    glDeleteBuffers(amountOfVBOBuffers, &vertexBufferObjects);
    glDeleteBuffers(amountOfEBOBuffers, &elementBufferObjects);
}