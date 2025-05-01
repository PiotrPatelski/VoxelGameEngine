#include "ChunkGraphics.hpp"
#include "VertexData.hpp"
#include <array>

namespace {
void initPositionVertexAttributes(unsigned int stride) {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
}

void initTextureCoordVertexAttributes(unsigned int stride) {
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(1);
}

void initNormalVertexAttributes(unsigned int stride) {
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
}
} // namespace

void ChunkGraphics::generateInstanceBuffersForCubeTypes() {
    const std::array<CubeType, 6> types{CubeType::SAND,  CubeType::DIRT,
                                        CubeType::GRASS, CubeType::WATER,
                                        CubeType::LOG,   CubeType::LEAVES};
    for (auto type : types) {
        unsigned id{};
        glGenBuffers(1, &id);
        instanceVBOs[type] = id;
    }
}

void ChunkGraphics::initializeGL(unsigned vbo, unsigned cubeEbo,
                                 unsigned waterEbo) {
    regularCubeEBO = cubeEbo;
    waterEBO = waterEbo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEbo);

    const unsigned stride = sizeof(Vertex);
    initPositionVertexAttributes(stride);
    initTextureCoordVertexAttributes(stride);
    initNormalVertexAttributes(stride);

    generateInstanceBuffersForCubeTypes();
    glBindVertexArray(0);
}

void ChunkGraphics::updateInstanceData(
    const std::unordered_map<CubeType, std::vector<glm::mat4>>& data) {
    instanceMatricesData = data;
    uploadInstanceBuffer();
}

void ChunkGraphics::uploadInstanceBuffer() {
    for (auto& [cubeType, instanceMatrices] : instanceMatricesData) {
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBOs[cubeType]);
        glBufferData(GL_ARRAY_BUFFER,
                     instanceMatrices.size() * sizeof(glm::mat4),
                     instanceMatrices.data(), GL_DYNAMIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ChunkGraphics::bindInstanceAttributesForType(CubeType cubeType) const {
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBOs.at(cubeType));
    for (unsigned attrIndex = 0; attrIndex < matrixAttrCount; ++attrIndex) {
        glVertexAttribPointer(3 + attrIndex, 4, GL_FLOAT, GL_FALSE,
                              sizeof(glm::mat4),
                              (void*)(attrIndex * sizeof(glm::vec4)));
        glEnableVertexAttribArray(3 + attrIndex);
        glVertexAttribDivisor(3 + attrIndex, 1);
    }
}

void ChunkGraphics::drawElements(CubeType cubeType, unsigned amt) const {
    if (cubeType == CubeType::WATER) {
        glDisable(GL_CULL_FACE);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
        glDrawElementsInstanced(GL_TRIANGLES,
                                static_cast<GLsizei>(waterIndicesCount),
                                GL_UNSIGNED_INT, nullptr, amt);
        glEnable(GL_CULL_FACE);
    } else {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, regularCubeEBO);
        glDrawElementsInstanced(GL_TRIANGLES,
                                static_cast<GLsizei>(indices.size()),
                                GL_UNSIGNED_INT, nullptr, amt);
    }
}

void ChunkGraphics::renderByType(CubeType cubeType) const {
    auto instanceMatrices = instanceMatricesData.find(cubeType);
    if (instanceMatrices == instanceMatricesData.end() or
        instanceMatrices->second.empty()) {
        return;
    }

    glBindVertexArray(vao);
    bindInstanceAttributesForType(cubeType);
    drawElements(cubeType,
                 static_cast<unsigned>(instanceMatrices->second.size()));
    glBindVertexArray(0);
}

ChunkGraphics::~ChunkGraphics() {
    for (auto& [_, bufferId] : instanceVBOs) {
        glDeleteBuffers(1, &bufferId);
    }
    if (vao) {
        glDeleteVertexArrays(1, &vao);
    }
}
