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
    const std::array<CubeType, 7> types{
        CubeType::SAND, CubeType::DIRT,   CubeType::GRASS, CubeType::WATER,
        CubeType::LOG,  CubeType::LEAVES, CubeType::TORCH};
    for (auto type : types) {
        unsigned cubeId{};
        unsigned lightSourceId{};
        glGenBuffers(1, &cubeId);
        glGenBuffers(1, &lightSourceId);
        instanceVBOs[type] = cubeId;
        instanceLightVBOs[type] = lightSourceId;
    }
}

void ChunkGraphics::initializeTorchLightVolumeGLParams(int volumeDimension) {
    glGenTextures(1, &lightVolumeTexture);
    glBindTexture(GL_TEXTURE_3D, lightVolumeTexture);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    float border[] = {0, 0, 0, 0};
    glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, border);
    // allocate (no data yet)
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, volumeDimension, volumeDimension,
                 volumeDimension, 0, GL_RED, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_3D, 0);
}

void ChunkGraphics::initializeGL(unsigned vertexBufferObjects, unsigned cubeEbo,
                                 unsigned waterEbo, int volumeDimension) {
    regularCubeEBO = cubeEbo;
    waterEBO = waterEbo;

    glGenVertexArrays(1, &vertexArrayObjects);
    glBindVertexArray(vertexArrayObjects);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEbo);

    const unsigned stride = sizeof(Vertex);
    initPositionVertexAttributes(stride);
    initTextureCoordVertexAttributes(stride);
    initNormalVertexAttributes(stride);

    generateInstanceBuffersForCubeTypes();
    initializeTorchLightVolumeGLParams(volumeDimension);
    glBindVertexArray(0);
}

void ChunkGraphics::updateInstanceData(
    const std::unordered_map<CubeType, std::vector<glm::mat4>>& matricesData) {
    instanceMatricesData = matricesData;
    uploadInstanceBuffer();
}

void ChunkGraphics::updateLightVolume(const std::vector<float>& volume,
                                      int volumeDimension) {
    // upload new 3D data
    glActiveTexture(GL_TEXTURE0 + 15);
    glBindTexture(GL_TEXTURE_3D, lightVolumeTexture);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, volumeDimension, volumeDimension,
                    volumeDimension, GL_RED, GL_FLOAT, volume.data());
    glBindTexture(GL_TEXTURE_3D, 0);
    glActiveTexture(GL_TEXTURE0);
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
    const unsigned lightAttr = 3 + matrixAttrCount;
    glBindBuffer(GL_ARRAY_BUFFER, instanceLightVBOs.at(cubeType));
    glVertexAttribPointer(lightAttr, 1, GL_FLOAT, GL_FALSE, sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(lightAttr);
    glVertexAttribDivisor(lightAttr, 1);
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

    glBindVertexArray(vertexArrayObjects);
    bindInstanceAttributesForType(cubeType);
    glActiveTexture(GL_TEXTURE0 + 15);
    glBindTexture(GL_TEXTURE_3D, lightVolumeTexture);
    glActiveTexture(GL_TEXTURE0);
    drawElements(cubeType,
                 static_cast<unsigned>(instanceMatrices->second.size()));
    glBindVertexArray(0);
}

ChunkGraphics::~ChunkGraphics() {
    for (auto& [_, buf] : instanceVBOs) {
        glDeleteBuffers(1, &buf);
    }
    for (auto& [_, buf] : instanceLightVBOs) {
        glDeleteBuffers(1, &buf);
    }
    if (lightVolumeTexture) {
        glDeleteTextures(1, &lightVolumeTexture);
    }
    if (vertexArrayObjects) {
        glDeleteVertexArrays(1, &vertexArrayObjects);
    }
}
