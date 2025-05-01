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

    void initializeGL(unsigned sharedVBO, unsigned sharedCubeEBO,
                      unsigned sharedWaterEBO);

    void updateInstanceData(
        const std::unordered_map<CubeType, std::vector<glm::mat4>>&);

    void renderByType(CubeType type) const;

   private:
    void generateInstanceBuffersForCubeTypes();
    void bindInstanceAttributesForType(CubeType type) const;
    void uploadInstanceBuffer();
    void drawElements(CubeType type, unsigned amount) const;

    std::unordered_map<CubeType, std::vector<glm::mat4>> instanceMatricesData{};
    std::unordered_map<CubeType, unsigned> instanceVBOs{};

    unsigned vao{0}, regularCubeEBO{0}, waterEBO{0};
    static constexpr unsigned matrixAttrCount = 4;
};
