#include "WaterMeshBuilder.hpp"
#include "WaterSystem.hpp"
#include <queue>

namespace {
constexpr int VERTICES_PER_QUAD{4};
constexpr int INDICES_PER_QUAD{6};
constexpr float WATER_SURFACE_OFFSET{0.5f};
constexpr float QUAD_HALF_SIZE{0.5f};
constexpr glm::vec3 WATER_SURFACE_NORMAL{0.0f, 1.0f, 0.0f};

constexpr glm::vec2 TEX_COORD_BOTTOM_LEFT{0.0f, 0.0f};
constexpr glm::vec2 TEX_COORD_BOTTOM_RIGHT{1.0f, 0.0f};
constexpr glm::vec2 TEX_COORD_TOP_RIGHT{1.0f, 1.0f};
constexpr glm::vec2 TEX_COORD_TOP_LEFT{0.0f, 1.0f};

const std::vector<glm::ivec3> HORIZONTAL_OFFSETS = {
    {1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1}};
} // namespace

std::vector<WaterMeshBuilder::WaterSurface>
WaterMeshBuilder::buildWaterSurfaces(const VoxelTypes::VoxelGrid3D& voxelGrid,
                                     float chunkWorldX, float chunkWorldZ) {
    const int chunkSize = static_cast<int>(voxelGrid.size());
    initializeProcessedGrid(chunkSize);
    return collectWaterSurfaces(voxelGrid, chunkSize, chunkWorldX, chunkWorldZ);
}

std::vector<WaterMeshBuilder::WaterSurface>
WaterMeshBuilder::collectWaterSurfaces(const VoxelTypes::VoxelGrid3D& voxelGrid,
                                       int chunkSize, float chunkWorldX,
                                       float chunkWorldZ) {
    std::vector<WaterSurface> waterSurfaces;

    for (int x = 0; x < chunkSize; ++x) {
        for (int z = 0; z < chunkSize; ++z) {
            for (int y = 0; y < chunkSize; ++y) {
                if (isValidWaterPosition(voxelGrid, {x, y, z})) {
                    auto surface = buildConnectedSurface(
                        voxelGrid, {x, y, z}, chunkWorldX, chunkWorldZ);
                    if (not surface.vertices.empty()) {
                        waterSurfaces.push_back(std::move(surface));
                    }
                }
            }
        }
    }

    return waterSurfaces;
}

WaterMeshBuilder::WaterSurface WaterMeshBuilder::buildConnectedSurface(
    const VoxelTypes::VoxelGrid3D& voxelGrid, const glm::ivec3& position,
    float chunkWorldX, float chunkWorldZ) {
    WaterSurface surface;
    surface.waterType = voxelGrid[position.x][position.z][position.y];

    generateUnifiedMesh(
        createWaterSurface(voxelGrid, position, surface.waterType), chunkWorldX,
        chunkWorldZ, surface);

    return surface;
}

void WaterMeshBuilder::initializeProcessedGrid(int chunkSize) {
    processed.assign(chunkSize,
                     std::vector<std::vector<bool>>(
                         chunkSize, std::vector<bool>(chunkSize, false)));
}

bool WaterMeshBuilder::isValidWaterPosition(
    const VoxelTypes::VoxelGrid3D& voxelGrid,
    const glm::ivec3& position) const {
    const int chunkSize = static_cast<int>(voxelGrid.size());

    if (position.x < 0 or position.x >= chunkSize or position.y < 0 or
        position.y >= chunkSize or position.z < 0 or position.z >= chunkSize) {
        return false;
    }

    if (processed[position.x][position.z][position.y]) {
        return false;
    }

    CubeType cubeType = voxelGrid[position.x][position.z][position.y];
    return WaterSystem::isWater(cubeType);
}

std::vector<glm::ivec3> WaterMeshBuilder::createWaterSurface(
    const VoxelTypes::VoxelGrid3D& voxelGrid, const glm::ivec3& position,
    CubeType waterType) {
    std::vector<glm::ivec3> surfaceBlocks;
    std::queue<glm::ivec3> queue;
    queue.push(position);
    processed[position.x][position.z][position.y] = true;

    while (not queue.empty()) {
        glm::ivec3 current = queue.front();
        queue.pop();

        surfaceBlocks.push_back(current);

        for (const auto& offset : HORIZONTAL_OFFSETS) {
            glm::ivec3 neighbor = current + offset;

            if (isValidWaterPosition(voxelGrid, neighbor) and
                voxelGrid[neighbor.x][neighbor.z][neighbor.y] == waterType) {
                processed[neighbor.x][neighbor.z][neighbor.y] = true;
                queue.push(neighbor);
            }
        }
    }
    return surfaceBlocks;
}

void WaterMeshBuilder::generateUnifiedMesh(
    const std::vector<glm::ivec3>& surfaceBlocks, float chunkWorldX,
    float chunkWorldZ, WaterSurface& surface) {
    if (surfaceBlocks.empty()) {
        return;
    }

    surface.center =
        calculateSurfaceCenter(surfaceBlocks, chunkWorldX, chunkWorldZ);

    surface.vertices.reserve(surfaceBlocks.size() * VERTICES_PER_QUAD);
    surface.indices.reserve(surfaceBlocks.size() * INDICES_PER_QUAD);

    for (const auto& block : surfaceBlocks) {
        glm::vec3 worldPos =
            calculateWorldPosition(block, chunkWorldX, chunkWorldZ);
        addWaterQuad(worldPos, surface);
    }
}

glm::vec3 WaterMeshBuilder::calculateSurfaceCenter(
    const std::vector<glm::ivec3>& surfaceBlocks, float chunkWorldX,
    float chunkWorldZ) const {
    glm::vec3 centerSum(0.0f);
    for (const auto& block : surfaceBlocks) {
        centerSum += calculateWorldPosition(block, chunkWorldX, chunkWorldZ);
    }
    return centerSum / static_cast<float>(surfaceBlocks.size());
}

glm::vec3 WaterMeshBuilder::calculateWorldPosition(const glm::ivec3& block,
                                                   float chunkWorldX,
                                                   float chunkWorldZ) const {
    return glm::vec3(chunkWorldX + block.x, block.y, chunkWorldZ + block.z);
}

std::vector<Vertex> WaterMeshBuilder::createWaterQuadVertices(
    const glm::vec3& position) const {
    const float y = position.y + WATER_SURFACE_OFFSET;
    const glm::vec3 normal = WATER_SURFACE_NORMAL;

    const glm::vec3 bottomLeft{position.x - QUAD_HALF_SIZE, y,
                               position.z - QUAD_HALF_SIZE};
    const glm::vec3 bottomRight{position.x + QUAD_HALF_SIZE, y,
                                position.z - QUAD_HALF_SIZE};
    const glm::vec3 topRight{position.x + QUAD_HALF_SIZE, y,
                             position.z + QUAD_HALF_SIZE};
    const glm::vec3 topLeft{position.x - QUAD_HALF_SIZE, y,
                            position.z + QUAD_HALF_SIZE};

    return {{bottomLeft, TEX_COORD_BOTTOM_LEFT, normal},
            {bottomRight, TEX_COORD_BOTTOM_RIGHT, normal},
            {topRight, TEX_COORD_TOP_RIGHT, normal},
            {topLeft, TEX_COORD_TOP_LEFT, normal}};
}

void WaterMeshBuilder::addWaterQuad(const glm::vec3& position,
                                    WaterSurface& surface) {
    const unsigned int baseIndex =
        static_cast<unsigned int>(surface.vertices.size());

    const std::vector<Vertex> quadVertices = createWaterQuadVertices(position);
    surface.vertices.insert(surface.vertices.end(), quadVertices.begin(),
                            quadVertices.end());

    const std::vector<unsigned int> quadIndices = {baseIndex,     baseIndex + 1,
                                                   baseIndex + 2, baseIndex + 2,
                                                   baseIndex + 3, baseIndex};

    surface.indices.insert(surface.indices.end(), quadIndices.begin(),
                           quadIndices.end());
}
