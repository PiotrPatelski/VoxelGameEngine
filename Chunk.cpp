#include "Chunk.hpp"
#include "FastNoiseLite.h"

static constexpr int chunkSize{64};

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
    // Top face (y = -0.5)
    16, 17, 18, 18, 19, 16,
    // Bottom face (y = 0.5)
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

    std::vector<std::vector<std::vector<bool>>> cubePresent(
        chunkSize, std::vector<std::vector<bool>>(
                       chunkSize, std::vector<bool>(chunkSize, false)));

    // Also store column heights for each (x,z)
    std::vector<std::vector<int>> columnHeights(chunkSize,
                                                std::vector<int>(chunkSize, 0));

    const float initialCubeX =
        static_cast<float>(chunkWorldXPosition * chunkSize);
    const float initialCubeZ =
        static_cast<float>(chunkWorldZPosition * chunkSize);

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.01f);

    // First pass: mark cube presence based on noise.
    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            int worldCubeX = chunkWorldXPosition * chunkSize + x;
            int worldCubeZ = chunkWorldZPosition * chunkSize + z;
            float heightValue = noise.GetNoise(static_cast<float>(worldCubeX),
                                               static_cast<float>(worldCubeZ));
            int height =
                static_cast<int>((heightValue + 1.0f) * 0.5f * chunkSize / 2);
            columnHeights[x][z] = height;
            for (int y = 0; y < chunkSize; y++) {
                if (y <= height) cubePresent[x][z][y] = true;
            }
        }
    }

    // Second pass: add only cubes that are exposed (neighbors missing)
    // We'll use a local vector for visible cubes.
    std::vector<glm::mat4> initialModels;
    for (int x = 0; x < chunkSize; x++) {
        for (int z = 0; z < chunkSize; z++) {
            for (int y = 0; y < chunkSize; y++) {
                if (!cubePresent[x][z][y]) continue;
                bool exposed = false;
                // Check 6 neighbors. Our 3D array is indexed as [x][z][y]
                const int dirs[6][3] = {
                    {1, 0, 0},  {-1, 0, 0}, {0, 1, 0}, // z+
                    {0, -1, 0},                        // z-
                    {0, 0, 1},                         // y+
                    {0, 0, -1}                         // y-
                };
                for (int i = 0; i < 6; i++) {
                    int nx = x + dirs[i][0];
                    int nz = z + dirs[i][1];
                    int ny = y + dirs[i][2];
                    if (nx < 0 || nx >= chunkSize || nz < 0 ||
                        nz >= chunkSize || ny < 0 || ny >= chunkSize) {
                        exposed = true;
                        break;
                    }
                    if (!cubePresent[nx][nz][ny]) {
                        exposed = true;
                        break;
                    }
                }
                if (exposed) {
                    // Compute world position.
                    glm::vec3 cubePos{initialCubeX + static_cast<float>(x),
                                      static_cast<float>(y),
                                      initialCubeZ + static_cast<float>(z)};
                    // Create cube and add its model matrix.
                    cubes.push_back(std::make_unique<Cube>(cubePos));
                    initialModels.push_back(cubes.back()->getModel());
                }
            }
        }
    }
    modelMatrices = initialModels;
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

void Chunk::render(Shader& shader) {
    glBindVertexArray(vao);
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0,
                            cubes.size());
    glBindVertexArray(0);
}

void Chunk::performFrustumCulling(const Frustum& frustum) {
    modelMatrices.clear();
    for (const auto& cube : cubes) {
        if (cube) {
            glm::mat4 model = cube->getModel();
            glm::vec3 pos = glm::vec3(model[3]); // Extract translation.
            glm::vec3 aabbMin = pos - glm::vec3(0.5f);
            glm::vec3 aabbMax = pos + glm::vec3(0.5f);
            if (isAABBInsideFrustum(aabbMin, aabbMax, frustum)) {
                modelMatrices.push_back(model);
            }
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4),
                 modelMatrices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Chunk::updateInstanceBuffer() {
    modelMatrices.clear();
    for (const auto& cube : cubes) {
        if (cube) {
            modelMatrices.push_back(cube->getModel());
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4),
                 modelMatrices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Chunk::~Chunk() {}
