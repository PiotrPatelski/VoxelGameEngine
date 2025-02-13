#include "Chunk.hpp"
#include "FastNoiseLite.h"

static constexpr int chunkSize{64};

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

bool isOutOfChunk(int value) {
    return ((value >= static_cast<int>(chunkSize)) or (value < 0));
}

Chunk::Chunk(int worldXindex, int worldZindex)
    : chunkWorldXPosition{worldXindex}, chunkWorldZPosition{worldZindex} {
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.01f);
    const auto initialCubeX = static_cast<float>(worldXindex * chunkSize);
    const auto initialCubeZ = static_cast<float>(worldZindex * chunkSize);
    glm::vec3 currentCubePos{initialCubeX, 0.0f, initialCubeZ};
    for (int x{0}; x < chunkSize; x++) {
        cubes.emplace_back(std::vector<std::vector<std::unique_ptr<Cube>>>{});
        for (int z{0}; z < chunkSize; z++) {
            int worldCubeXPosition{
                chunkWorldXPosition * static_cast<int>(chunkSize) + x};
            int worldCubeZPosition{
                chunkWorldZPosition * static_cast<int>(chunkSize) + z};

            // Generate height using Perlin noise (normalized)
            const auto heightValue =
                noise.GetNoise(static_cast<float>(worldCubeXPosition),
                               static_cast<float>(worldCubeZPosition));
            const auto height =
                static_cast<int>((heightValue + 1.0f) * 0.5f * chunkSize /
                                 2); // Normalize to chunk size

            cubes[x].emplace_back(std::vector<std::unique_ptr<Cube>>{});
            for (int y{0}; y < chunkSize; y++) {
                if (y <= height) {
                    cubes[x][z].emplace_back(
                        std::make_unique<Cube>(currentCubePos));
                } else {
                    cubes[x][z].emplace_back(nullptr);
                }
                currentCubePos.y += 1.0f;
            }
            currentCubePos.z += 1.0f;
            currentCubePos.y = 0.0f;
        }
        currentCubePos.x += 1.0f;
        currentCubePos.z = initialCubeZ;
    }
}

bool Chunk::isSurroundedCube(const Cube& cube) const {
    const auto& cubePosition = cube.getPosition();
    const auto leftCubeIndex = static_cast<int>(
        cubePosition.x - chunkSize * chunkWorldXPosition - 1.0f);
    if (not isOutOfChunk(leftCubeIndex) and
        cubes[leftCubeIndex][cubePosition.z - chunkSize * chunkWorldZPosition]
             [cubePosition.y] == nullptr) {
        // No cube from left side
        return false;
    }
    const auto rightCubeIndex = static_cast<int>(
        cubePosition.x - chunkSize * chunkWorldXPosition + 1.0f);
    if (not isOutOfChunk(rightCubeIndex) and
        cubes[rightCubeIndex][cubePosition.z - chunkSize * chunkWorldZPosition]
             [cubePosition.y] == nullptr) {
        // No cube from right side
        return false;
    }
    const auto frontCubeIndex = static_cast<int>(
        cubePosition.z - chunkSize * chunkWorldZPosition - 1.0f);
    if (not isOutOfChunk(frontCubeIndex) and
        cubes[cubePosition.x - chunkSize * chunkWorldXPosition][frontCubeIndex]
             [cubePosition.y] == nullptr) {
        // No cube from front side
        return false;
    }
    const auto backCubeIndex = static_cast<int>(
        cubePosition.z - chunkSize * chunkWorldZPosition + 1.0f);
    if (not isOutOfChunk(backCubeIndex) and
        cubes[cubePosition.x - chunkSize * chunkWorldXPosition][backCubeIndex]
             [cubePosition.y] == nullptr) {
        // No cube from back side
        return false;
    }
    const auto bottomCubeIndex = static_cast<int>(cubePosition.y - 1.0f);
    if (not isOutOfChunk(bottomCubeIndex) and
        cubes[cubePosition.x - chunkSize * chunkWorldXPosition]
             [cubePosition.z - chunkSize * chunkWorldZPosition]
             [bottomCubeIndex] == nullptr) {
        // No cube from bottom side
        return false;
    }
    const auto topCubeIndex = static_cast<int>(cubePosition.y + 1.0f);
    if (not isOutOfChunk(topCubeIndex) and
        cubes[cubePosition.x - chunkSize * chunkWorldXPosition]
             [cubePosition.z - chunkSize * chunkWorldZPosition][topCubeIndex] ==
            nullptr) {
        // No cube from top side
        return false;
    }
    return true;
}

void Chunk::render(Shader& shader) {
    for (auto& x : cubes) {
        for (auto& y : x) {
            for (auto& z : y) {
                if (z and not isSurroundedCube(*z)) {
                    // calculate the model matrix for each object and pass it to
                    // shader before drawing make sure to initialize matrix to
                    // identity matrix first
                    shader.setMat4("model", z->getModel());
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }
    }
}

Chunk::~Chunk() {}