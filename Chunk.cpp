#include "Chunk.hpp"
#include "FastNoiseLite.h"

static constexpr unsigned int chunkSize{64};

// float vertices[] = {
//     // positions          // colors           // texture coords
//     0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
//     0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
//     -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
//     -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
// };
// unsigned int indices[] = {
//     0, 1, 3, // first triangle
//     1, 2, 3  // second triangle
// };

// clang-format off
//Pos: {X, Y, Z} Color:{R, G, B} TexCord:{X, Y} Normal{X, Y, Z}
std::vector<float> Chunk::vertices = {
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
    0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
    0.5f,  0.5f,  -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
    0.5f,  0.5f,  -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
    -0.5f, 0.5f,  -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

    -0.5f, -0.5f, 0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f,  -0.5f, 0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 
    0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 
    -0.5f, -0.5f, 0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 
    -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 
    0.5f,  0.5f,  -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    0.5f,  -0.5f, -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 
    0.5f,  -0.5f, -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    0.5f,  -0.5f, 0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 
    0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 
    0.5f,  -0.5f, -0.5f, 0.5f, 0.0f, 0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
    0.5f,  -0.5f, 0.5f,  0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 
    0.5f,  -0.5f, 0.5f,  0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f,  0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 
    -0.5f, -0.5f, -0.5f, 0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

    -0.5f, 0.5f,  -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 
    0.5f,  0.5f,  -0.5f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  0.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 
    0.5f,  0.5f,  0.5f,  0.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f,  0.5f,  0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 
    -0.5f, 0.5f,  -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
// clang-format on

Chunk::Chunk() {
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.01f);

    glm::vec3 currentCubePos{0.0f, 0.0f, 0.0f};
    for (unsigned int x{0}; x <= chunkSize; x++) {
        cubes.emplace_back(std::vector<std::vector<Cube>>{});
        for (unsigned int z{0}; z <= chunkSize; z++) {
            // Generate height using Perlin noise (normalized)
            const auto heightValue =
                noise.GetNoise(static_cast<float>(x), static_cast<float>(z));
            const auto height = static_cast<unsigned int>(
                (heightValue + 1.0f) * 0.5f * chunkSize /
                2); // Normalize to chunk size

            cubes[x].emplace_back(std::vector<Cube>{});
            for (unsigned int y{0}; y <= chunkSize; y++) {
                if (y <= height) {
                    cubes[x][z].emplace_back(Cube(currentCubePos));
                }
                currentCubePos.y += 1.0f;
            }
            currentCubePos.z += 1.0f;
            currentCubePos.y = 0.0f;
        }
        currentCubePos.x += 1.0f;
        currentCubePos.z = 0.0f;
    }
}

void Chunk::render(Shader& shader) {
    for (auto& x : cubes) {
        for (auto& y : x) {
            for (auto& z : y) {
                // calculate the model matrix for each object and pass it to
                // shader before drawing make sure to initialize matrix to
                // identity matrix first
                shader.setMat4("model", z.getModel());
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
    }
}

Chunk::~Chunk() {}