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
std::vector<Vertex> Chunk::vertices = {
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{0.5f,  0.5f,  -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
    {{0.5f,  -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{-0.5f, 0.5f,  -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},

    {{-0.5f, -0.5f, 0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f},{ 0.0f, 0.0f, 1.0f}},
    {{0.5f,  -0.5f, 0.5f},  {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f},{ 0.0f, 0.0f, 1.0f}},
    {{0.5f,  0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f},{ 0.0f, 0.0f, 1.0f}}, 
    {{-0.5f, 0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f},{ 0.0f, 0.0f, 1.0f}}, 

    {{-0.5f, 0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f},{-1.0f, 0.0f, 0.0f}},
    {{-0.5f, 0.5f, -0.5f},  {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f},{-1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f,-0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f},{-1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f},{-1.0f, 0.0f, 0.0f}},

    {{0.5f,  0.5f,  0.5f},  {0.5f, 0.5f, 0.0f}, {1.0f, 0.0f},{ 1.0f, 0.0f, 0.0f}},
    {{0.5f,  -0.5f,-0.5f},  {0.5f, 0.5f, 0.0f}, {0.0f, 1.0f},{ 1.0f, 0.0f, 0.0f}},
    {{0.5f,  0.5f, -0.5f},  {0.5f, 0.5f, 0.0f}, {1.0f, 1.0f},{ 1.0f, 0.0f, 0.0f}},
    {{0.5f,  -0.5f, 0.5f},  {0.5f, 0.5f, 0.0f}, {0.0f, 0.0f},{ 1.0f, 0.0f, 0.0f}},

    {{-0.5f, -0.5f,-0.5f},  {0.5f, 0.0f, 0.5f}, {0.0f, 1.0f},{ 0.0f, -1.0f, 0.0f}},
    {{0.5f,  -0.5f,-0.5f},  {0.5f, 0.0f, 0.5f}, {1.0f, 1.0f},{ 0.0f, -1.0f, 0.0f}},
    {{0.5f,  -0.5f, 0.5f},  {0.5f, 0.0f, 0.5f}, {1.0f, 0.0f},{ 0.0f, -1.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f},  {0.5f, 0.0f, 0.5f}, {0.0f, 0.0f},{ 0.0f, -1.0f, 0.0f}},

    {{-0.5f, 0.5f, -0.5f},  {0.0f, 0.5f, 0.5f}, {0.0f, 1.0f},{ 0.0f, 1.0f, 0.0f}},
    {{0.5f,  0.5f,  0.5f},  {0.0f, 0.5f, 0.5f}, {1.0f, 0.0f},{ 0.0f, 1.0f, 0.0f}},
    {{0.5f,  0.5f, -0.5f},  {0.0f, 0.5f, 0.5f}, {1.0f, 1.0f},{ 0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f,  0.5f},  {0.0f, 0.5f, 0.5f}, {0.0f, 0.0f},{ 0.0f, 1.0f, 0.0f}}};

std::vector<unsigned int> Chunk::indices = {
    // Front face (z = -0.5)
    0, 1, 2, 1, 0, 3,
    // Back face (z = 0.5)
    4, 5, 6, 6, 7, 4,
    // Left face (x = -0.5)
    8, 9, 10, 10, 11, 8,
    // Right face (x = 0.5)
    12, 13, 14, 13, 12, 15,
    // Top face (y = 0.5)
    16, 17, 18, 18, 19, 16,
    // Bottom face (y = -0.5)
    20, 21, 22, 21, 20, 23
};
// clang-format on

Chunk::Chunk(int worldXindex, int worldZindex, unsigned int sharedVBO,
             unsigned int sharedEBO)
    : chunkWorldXPosition{worldXindex}, chunkWorldZPosition{worldZindex} {
    // First, create the VAO and bind the shared vertex data.
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Bind shared VBO and EBO
    glBindBuffer(GL_ARRAY_BUFFER, sharedVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sharedEBO);

    const unsigned int stride = sizeof(Vertex);
    // POSITION attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    // COLOR attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);
    // TEXCOORD attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);
    // NORMAL attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(3);

    // Unbind the array buffer (EBO remains bound to the VAO)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Generate the instance buffer (we will fill it later)
    glGenBuffers(1, &buffer);

    // We'll bind the instance buffer later after updating the model matrices.
    glBindVertexArray(0);

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
                    cubes.push_back(std::make_unique<Cube>(currentCubePos));
                    // TEMPORARY LOGGING:
                    if (x == chunkSize - 1 || x == 0) {
                        // For boundary columns
                        std::cout
                            << "Chunk (" << worldXindex << "," << worldZindex
                            << "), cube at (" << currentCubePos.x << ", "
                            << currentCubePos.y << ", " << currentCubePos.z
                            << ")\n";
                    }
                    modelMatrices.push_back(cubes.back()->getModel());
                }
                currentCubePos.y += 1.0f;
            }
            currentCubePos.z += 1.0f;
            currentCubePos.y = 0.0f;
        }
        currentCubePos.x += 1.0f;
        currentCubePos.z = initialCubeZ;
    }

    // Now update the instance buffer with the computed model matrices.
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4),
                 modelMatrices.data(), GL_STATIC_DRAW);

    // Bind the VAO again and set up the instanced attribute pointers.
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    // A mat4 is sent as 4 vec4 attributes (locations 4, 5, 6, and 7)
    for (unsigned int i = 0; i < 4; i++) {
        glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                              (void*)(i * sizeof(glm::vec4)));
        glEnableVertexAttribArray(4 + i);
        glVertexAttribDivisor(4 + i, 1); // This makes the attribute instanced.
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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
    // unsigned index{0};
    // printf("cubes.size = %d\n", static_cast<int>(cubes.size()));
    glBindVertexArray(vao);
    // glBindBuffer(GL_ARRAY_BUFFER, buffer);
    // for (auto& cube : cubes) {
    //     std::ignore = cube;
    //     // if (cube and not isSurroundedCube(*cube))
    //     //  if (cube and boundingVolume.isOnFrustum(frustum,
    //     // cube->getModel()))
    //     //  {
    //     // if (cube) {
    //     // calculate the model matrix for each object and pass it to
    //     // shader before drawing make sure to initialize matrix to
    //     // identity matrix first
    //     // shader.setMat4("model", cube->getModel());
    //     // glDrawArrays(GL_TRIANGLES, 0, 36);
    //     // glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT,
    //     0);
    //     // printf("Try to render cubes\n");
    //     // glDrawElementsInstanced(GL_TRIANGLES, indices.size(),
    //     // GL_UNSIGNED_INT,
    //     //                         0, cubes.size());
    //     // }
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0,
                            cubes.size());
    //     // printf("success %d\n", index++);
    // }
    // glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0,
    //                         cubes.size());
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Chunk::applyCubeModels() {
    size_t index{0};
    for (const auto& cube : cubes) {
        if (cube) {
            modelMatrices[index] = cube->getModel();
        }
        index++;
    }
}

Chunk::~Chunk() {}
