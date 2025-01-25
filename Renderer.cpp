#include "Renderer.hpp"
#include "Chunk.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace {
// settings
// BUFFER DEFAULTS
unsigned int vertexBufferObjects, vertexArrayObjects,
    elementBufferObjects; // must be overwritten before use
unsigned int lightSourceVAO;
const unsigned int amountOfVBOBuffers{1};
const unsigned int amountOfVAOBuffers{1};
const unsigned int amountOfEBOBuffers{1};
const unsigned int cubeVAOIndex{0};

Chunk chunk{};

// LIGHTING
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

std::vector<glm::vec3> pointLightPositions = {
    glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.0f, 0.0f, -3.0f)};

void setupVertexBufferData() {
    // FIRST CUBES
    //  set up vertex data (and buffer(s)) and configure vertex attributes
    //  ------------------------------------------------------------------
    glGenBuffers(amountOfVBOBuffers, &vertexBufferObjects);
    glGenVertexArrays(amountOfVAOBuffers, &vertexArrayObjects);
    glGenBuffers(amountOfEBOBuffers, &elementBufferObjects);

    glBindVertexArray(vertexArrayObjects);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects);
    glBufferData(GL_ARRAY_BUFFER, Chunk::getVertices().size() * sizeof(float),
                 Chunk::getVertices().data(), GL_STATIC_DRAW);
    const unsigned int stride = 11 * sizeof(float);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObjects);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
    //              GL_STATIC_DRAW);
    // glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean
    // normalized, GLsizei stride, const void *pointer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                          (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride,
                          (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    // SECOND LIGHT
    glGenVertexArrays(amountOfVAOBuffers, &lightSourceVAO);
    glBindVertexArray(lightSourceVAO);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                          (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //DRAW LINES ONLY - FOR DEBUG
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

unsigned int createTexture(const std::string& path) {
    unsigned int texture;
    glGenTextures(1, &texture);

    int width, height, nrChannels;
    unsigned char* data =
        stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        const float borderColor[] = {1.0f, 1.0f, 0.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        // GL_LINEAR for smoother look from distance
        // GL_NEAREST for more detailed zoom
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
    std::cout << "created texture with id: " << texture << std::endl;
    return texture;
}

} // namespace

Renderer::Renderer(unsigned int width, unsigned int height)
    : screenWidth{static_cast<float>(width)},
      screenHeight{static_cast<float>(height)} {
    std::cout << "Renderer::Init!" << std::endl;

    setupVertexBufferData();

    // load and create a texture
    // -------------------------
    stbi_set_flip_vertically_on_load(true);
    texture1 = createTexture("./textures/container2.png");
    texture2 = createTexture("./textures/awesomeface.png");
    specularMapContainer = createTexture("./textures/container2_specular.png");
    emissionMap = createTexture("./textures/matrix.jpg");
    cubeShader = std::make_unique<Shader>("shaders/cube_shader.vs",
                                          "shaders/cube_shader.fs");
    lightCubeShader = std::make_unique<Shader>("shaders/light_source.vs",
                                               "shaders/light_source.fs");
    // lightCubeShader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    // lightCubeShader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    cubeShader->use();
    // cubeShader->setInt("texture1", 0);
    cubeShader->setInt("material.diffuse", 0);
    cubeShader->setInt("material.specular", 2);
    // cubeShader->setInt("texture2", 1);
    cubeShader->setInt("material.emission", 3);
    cubeShader->setFloat("fadeValue", 0.2f);
    // cubeShader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    // cubeShader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);

    cubeShader->setFloat("material.shininess", 32.0f);
    /*
       Here we set all the uniforms for the 5/6 types of lights we have. We
       have to set them manually and index the proper PointLight struct in
       the array to set each uniform variable. This can be done more
       code-friendly by defining light types as classes and set their values
       in there, or by using a more efficient uniform approach by using
       'Uniform buffer objects', but that is something we'll discuss in the
       'Advanced GLSL' tutorial.
    */
    // directional light
    cubeShader->setVec3("directionalLight.direction", -0.2f, -1.0f, -0.3f);
    cubeShader->setVec3("directionalLight.ambient", 0.0f, 0.0f, 0.0f);
    cubeShader->setVec3("directionalLight.diffuse", 0.05f, 0.05f, 0.05f);
    cubeShader->setVec3("directionalLight.specular", 0.2f, 0.2f, 0.2f);
    // point lights
    for (unsigned int lightPosIndex = 0;
         lightPosIndex < pointLightPositions.size(); lightPosIndex++) {
        const std::string uniformName =
            "pointLights[" + std::to_string(lightPosIndex) + "]";
        cubeShader->setVec3(uniformName + ".position",
                            pointLightPositions[lightPosIndex]);
        cubeShader->setVec3(uniformName + ".ambient", 0.05f, 0.05f, 0.05f);
        cubeShader->setVec3(uniformName + ".diffuse", 0.8f, 0.8f, 0.8f);
        cubeShader->setVec3(uniformName + ".specular", 1.0f, 1.0f, 1.0f);
        cubeShader->setFloat(uniformName + ".constant", 1.0f);
        cubeShader->setFloat(uniformName + ".linear", 0.09f);
        cubeShader->setFloat(uniformName + ".quadratic", 0.032f);
    }
    cubeShader->setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    cubeShader->setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    cubeShader->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    cubeShader->setFloat("spotLight.constant", 1.0f);
    cubeShader->setFloat("spotLight.linear", 0.09f);
    cubeShader->setFloat("spotLight.quadratic", 0.032f);
    cubeShader->setFloat("spotLight.innerCutOff",
                         glm::cos(glm::radians(12.5f)));
    cubeShader->setFloat("spotLight.outerCutOff",
                         glm::cos(glm::radians(15.0f)));

    glActiveTexture(GL_TEXTURE0); // activate the texture unit first before
                                  // binding texture
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, texture2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, specularMapContainer);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, emissionMap);
}

Renderer::~Renderer() {
    std::cout << "Renderer::Shutdown!" << std::endl;
    // CLEANUP
    glDeleteVertexArrays(amountOfVAOBuffers, &vertexArrayObjects);
    glDeleteBuffers(amountOfVBOBuffers, &vertexBufferObjects);
    glDeleteBuffers(amountOfEBOBuffers, &elementBufferObjects);
    glfwTerminate();
}

void Renderer::updateShaders(const Camera& camera) {
    // lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f; //position rotation in
    // a circle lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;

    cubeShader->use();
    cubeShader->setFloat("time", glfwGetTime());
    cubeShader->setVec3("viewPosition", camera.getPosition());

    // spotLight
    cubeShader->setVec3("spotLight.position", camera.getPosition());
    cubeShader->setVec3("spotLight.direction", camera.getFront());

    glm::mat4 projection =
        glm::perspective(glm::radians(camera.getZoom()),
                         screenWidth / screenHeight, 0.1f, 100.0f);
    glm::mat4 cameraView = camera.getViewMatrix();
    cubeShader->setMat4("projection", projection);
    cubeShader->setMat4("view", cameraView);

    lightCubeShader->use();
    glm::vec3 lightSourceColor(1.0f, 0.5f * sin(glfwGetTime() * 3.0f),
                               0.5f * sin(glfwGetTime() * 3.0f));
    lightCubeShader->setVec3("objectColor", lightSourceColor);
    lightCubeShader->setMat4("projection", projection);
    lightCubeShader->setMat4("view", cameraView);
}

void Renderer::render() {
    // RENDER
    glClearColor(0.2f, 0.5f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render boxes
    glBindVertexArray(vertexArrayObjects);
    cubeShader->use();
    chunk.render(*cubeShader);

    lightCubeShader->use();
    glBindVertexArray(lightSourceVAO);
    for (const auto& position : pointLightPositions) {
        auto lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, position);
        lightModel = glm::scale(lightModel, glm::vec3(0.2f));
        lightCubeShader->setMat4("model", lightModel);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}