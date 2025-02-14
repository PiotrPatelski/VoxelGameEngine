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
    0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
    0.5f,  -0.5f,  -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
    0.5f,  0.5f,  -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f,  -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
    -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,

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
    0.5f,  -0.5f,  -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 
    0.5f,  -0.5f, -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    0.5f,  0.5f, 0.5f,  0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 
    0.5f,  -0.5f,  0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 
    0.5f,  -0.5f, -0.5f, 0.5f, 0.0f, 0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
    0.5f,  -0.5f, 0.5f,  0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 
    0.5f,  -0.5f, 0.5f,  0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f,  0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 
    -0.5f, -0.5f, -0.5f, 0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

    -0.5f, 0.5f,  -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 
    0.5f,  0.5f,  0.5f, 0.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f,  0.5f,  -0.5f,  0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 
    0.5f,  0.5f,  0.5f,  0.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f,  -0.5f,  0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 
    -0.5f, 0.5f,  0.5f, 0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};
// clang-format on

Chunk::Chunk(int worldXindex, int worldZindex)
    : chunkWorldXPosition{worldXindex}, chunkWorldZPosition{worldZindex} {
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.01f);
    const auto initialCubeX = static_cast<float>(worldXindex * chunkSize);
    const auto initialCubeZ = static_cast<float>(worldZindex * chunkSize);
    glm::vec3 currentCubePos{initialCubeX, 0.0f, initialCubeZ};
    for (int x{0}; x < chunkSize; x++) {
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

            for (int y{0}; y < chunkSize; y++) {
                if (y <= height) {
                    cubes.emplace_back(std::make_unique<Cube>(currentCubePos));
                } else {
                    cubes.emplace_back(nullptr);
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

bool Chunk::isCubeOnBorder(const glm::vec3& position) const {
    const glm::vec3 lowerBorder = {chunkWorldXPosition * chunkSize, 0,
                                   chunkWorldZPosition * chunkSize};
    const glm::vec3 upperBorder = {lowerBorder.x + chunkSize - 1, chunkSize - 1,
                                   lowerBorder.z + chunkSize - 1};
    const bool isXBorder =
        position.x <= lowerBorder.x or position.x >= upperBorder.x;
    const bool isYBorder = position.y <= 0 or position.y >= chunkSize - 1;
    const bool isZBorder =
        position.z <= lowerBorder.z or position.z >= upperBorder.z;
    return (isXBorder or isYBorder or isZBorder);
}

auto Chunk::positionToIndex(const glm::vec3& position) const {
    // ASSERT position values are in bounds of chunk's x,y,z
    const auto xFactor =
        (position.x - chunkWorldXPosition * chunkSize) * chunkSize * chunkSize;
    const auto zFactor =
        (position.z - chunkWorldZPosition * chunkSize) * chunkSize;
    const auto cubeIndex = xFactor + zFactor + position.y;
    return cubeIndex;
}

bool Chunk::isSurroundedCube(const Cube& cube) const {
    const auto& cubePosition = cube.getPosition();

    if (isCubeOnBorder(cubePosition)) {
        // TO BE HANDLED when reference to neighbouring chunks is added
        return false;
    }
    if (cubes[positionToIndex(cubePosition + glm::vec3{-1.0f, 0.f, 0.f})] ==
        nullptr) {
        return false;
    }
    if (cubes[positionToIndex(cubePosition + glm::vec3{1.0f, 0.f, 0.f})] ==
        nullptr) {
        return false;
    }
    if (cubes[positionToIndex(cubePosition + glm::vec3{0.0f, 0.f, -1.0f})] ==
        nullptr) {
        return false;
    }
    if (cubes[positionToIndex(cubePosition + glm::vec3{0.0f, 0.f, 1.0f})] ==
        nullptr) {
        return false;
    }
    if (cubes[positionToIndex(cubePosition + glm::vec3{0.0f, -1.0f, 0.0f})] ==
        nullptr) {
        return false;
    }
    if (cubes[positionToIndex(cubePosition + glm::vec3{0.0f, 1.0f, 0.0f})] ==
        nullptr) {
        return false;
    }
    return true;
}

void Chunk::render(Shader& shader) {
    for (auto& cube : cubes) {
        if (cube and not isSurroundedCube(*cube)) {
            // calculate the model matrix for each object and pass it to
            // shader before drawing make sure to initialize matrix to
            // identity matrix first
            shader.setMat4("model", cube->getModel());
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
}

Chunk::~Chunk() {}