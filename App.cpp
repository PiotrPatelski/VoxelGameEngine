#include "App.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <functional>
#include "Shader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace {
// settings
// SCREEN SIZE
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
// BUFFER DEFAULTS
unsigned int vertexBufferObjects, vertexArrayObjects,
    elementBufferObjects; // must be overwritten before use
unsigned int lightSourceVAO;
const unsigned int amountOfVBOBuffers{1};
const unsigned int amountOfVAOBuffers{1};
const unsigned int amountOfEBOBuffers{1};
const unsigned int cubeVAOIndex{0};
float faceFadeValue{0.2};
// DELTATIME
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;
bool firstMouse = true;
// LIGHTING
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina
    // displays.
    glViewport(0, 0, width, height);
}

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
float verticesCube[] = {
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

glm::vec3 cubePositions[] = {
    glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
    glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

glm::vec3 pointLightPositions[] = {
    glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.0f, 0.0f, -3.0f)};

GLFWwindow* createAppWindow() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //MAC OS ONLY!
    auto* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
                                    "OpenGL Pioter Test Window", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        std::abort();
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // setting mouse handling
    // --------------------
    glfwSetInputMode(window, GLFW_CURSOR,
                     GLFW_CURSOR_DISABLED); // HIDE THE CURSOR

    return window;
}

void setupVertexBufferData() {
    // FIRST CUBES
    //  set up vertex data (and buffer(s)) and configure vertex attributes
    //  ------------------------------------------------------------------
    glGenBuffers(amountOfVBOBuffers, &vertexBufferObjects);
    glGenVertexArrays(amountOfVAOBuffers, &vertexArrayObjects);
    glGenBuffers(amountOfEBOBuffers, &elementBufferObjects);

    glBindVertexArray(vertexArrayObjects);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCube), verticesCube,
                 GL_STATIC_DRAW);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObjects);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
    //              GL_STATIC_DRAW);
    // glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean
    // normalized, GLsizei stride, const void *pointer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                          (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                          (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    // SECOND LIGHT
    glGenVertexArrays(amountOfVAOBuffers, &lightSourceVAO);
    glBindVertexArray(lightSourceVAO);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                          (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
    //                       (void*)(6 * sizeof(float)));
    // glEnableVertexAttribArray(2);

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
    return texture;
}

} // namespace

App::App() {
    std::cout << "App::Init!" << std::endl;

    // glfw window creation
    // --------------------
    window = createAppWindow();

    // set input callbacks
    //-------------------
    // Associate the App instance with the GLFW window
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(
        window, [](GLFWwindow* window, double xpos, double ypos) {
            App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
            app->mouse_callback(window, xpos, ypos);
        });
    glfwSetScrollCallback(
        window, [](GLFWwindow* window, double xoffset, double yoffset) {
            App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
            app->scroll_callback(window, xoffset, yoffset);
        });

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

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
    cubeShader->setFloat("fadeValue", faceFadeValue);
    // cubeShader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    // cubeShader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
}

App::~App() {
    // CLEANUP
    glDeleteVertexArrays(amountOfVAOBuffers, &vertexArrayObjects);
    glDeleteBuffers(amountOfVBOBuffers, &vertexBufferObjects);
    glDeleteBuffers(amountOfEBOBuffers, &elementBufferObjects);

    glfwTerminate();
}

void App::run() {
    // MAIN LOOP
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // USER INPUT
        processInput(window);

        // RENDER
        glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f; //position rotation in
        // a circle lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;

        cubeShader->use();
        cubeShader->setFloat("time", glfwGetTime());
        cubeShader->setVec3("viewPosition", camera.getPosition());
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
        // point light 1
        cubeShader->setVec3("pointLights[0].position", pointLightPositions[0]);
        cubeShader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        cubeShader->setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        cubeShader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        cubeShader->setFloat("pointLights[0].constant", 1.0f);
        cubeShader->setFloat("pointLights[0].linear", 0.09f);
        cubeShader->setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        cubeShader->setVec3("pointLights[1].position", pointLightPositions[1]);
        cubeShader->setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        cubeShader->setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        cubeShader->setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        cubeShader->setFloat("pointLights[1].constant", 1.0f);
        cubeShader->setFloat("pointLights[1].linear", 0.09f);
        cubeShader->setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        cubeShader->setVec3("pointLights[2].position", pointLightPositions[2]);
        cubeShader->setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        cubeShader->setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        cubeShader->setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        cubeShader->setFloat("pointLights[2].constant", 1.0f);
        cubeShader->setFloat("pointLights[2].linear", 0.09f);
        cubeShader->setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        cubeShader->setVec3("pointLights[3].position", pointLightPositions[3]);
        cubeShader->setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        cubeShader->setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        cubeShader->setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        cubeShader->setFloat("pointLights[3].constant", 1.0f);
        cubeShader->setFloat("pointLights[3].linear", 0.09f);
        cubeShader->setFloat("pointLights[3].quadratic", 0.032f);
        // spotLight
        cubeShader->setVec3("spotLight.position", camera.getPosition());
        cubeShader->setVec3("spotLight.direction", camera.getFront());
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

        glm::mat4 projection = glm::perspective(
            glm::radians(camera.getZoom()),
            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 cameraView = camera.getViewMatrix();
        cubeShader->setMat4("projection", projection);
        cubeShader->setMat4("view", cameraView);

        glActiveTexture(GL_TEXTURE0); // activate the texture unit first before
                                      // binding texture
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, texture2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, specularMapContainer);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, emissionMap);

        // render boxes
        glBindVertexArray(vertexArrayObjects);
        for (unsigned int i = 0; i < 10; i++) {
            // calculate the model matrix for each object and pass it to shader
            // before drawing make sure to initialize matrix to identity matrix
            // first

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            angle = glfwGetTime() * 25.0f;
            model = glm::rotate(model, glm::radians(angle),
                                glm::vec3(1.0f, 0.3f, 0.5f));
            cubeShader->use();
            cubeShader->setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glm::vec3 lightSourceColor(1.0f, 0.5f * sin(glfwGetTime() * 3.0f),
                                   0.5f * sin(glfwGetTime() * 3.0f));

        lightCubeShader->use();
        lightCubeShader->setVec3("objectColor", lightSourceColor);
        lightCubeShader->setMat4("projection", projection);
        lightCubeShader->setMat4("view", cameraView);
        glBindVertexArray(lightSourceVAO);
        for (unsigned int i = 0; i < 4; i++) {
            auto lightModel = glm::mat4(1.0f);
            lightModel = glm::translate(lightModel, pointLightPositions[i]);
            lightModel = glm::scale(lightModel, glm::vec3(0.2f));
            lightCubeShader->setMat4("model", lightModel);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse
        // moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

//--------------------------------------------------------------------------
// window param present eventhough unused only for signature of dependent lib
void App::mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset =
        lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

//--------------------------------------------------------------------------
// window param present eventhough unused only for signature of dependent lib
void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processMouseScroll(static_cast<float>(yoffset));
}

void App::processInput(GLFWwindow* window) {
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
        faceFadeValue += 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
        faceFadeValue -= 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.processKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.processKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.processKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.processKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        lightPos.y += 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        lightPos.y -= 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        lightPos.x += 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        lightPos.x -= 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        lightPos.z += 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        lightPos.z -= 0.01;
    }
}