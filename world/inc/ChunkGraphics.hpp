#pragma once
#include <unordered_map>
#include <vector>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Cube.hpp"
#include "Shader.hpp"

/** Holds only GL state – all uploads & draw‑calls happen here. */
class ChunkGraphics {
   public:
    ChunkGraphics() = default;
    ~ChunkGraphics();

    ChunkGraphics(const ChunkGraphics&) = delete;
    ChunkGraphics& operator=(const ChunkGraphics&) = delete;
    ChunkGraphics(ChunkGraphics&&) = delete;
    ChunkGraphics& operator=(ChunkGraphics&&) = delete;

    void initializeGL(unsigned sharedVBO, unsigned sharedCubeEBO,
                      unsigned sharedWaterEBO, int volumeDimension);

    void updateInstanceData(
        const std::unordered_map<CubeType, std::vector<glm::mat4>>& matrices);
    void updateLightVolume(const std::vector<float>& volume,
                           int volumeDimension);

    void renderByType(CubeType type) const;

   private:
    void generateInstanceBuffersForCubeTypes();
    void initializeTorchLightVolumeGLParams(int volumeDimension);
    void bindInstanceAttributesForType(CubeType type) const;
    void uploadInstanceBuffer();
    void drawElements(CubeType type, unsigned amount) const;

    std::unordered_map<CubeType, std::vector<glm::mat4>> instanceMatricesData{};
    std::unordered_map<CubeType, unsigned> instanceVBOs{};
    std::unordered_map<CubeType, unsigned> instanceLightVBOs{};
    GLuint lightVolumeTexture{0};

    unsigned vertexArrayObjects{0}, regularCubeEBO{0}, waterEBO{0};
};
