#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include "Shader.hpp"
#include "Camera.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

// GLOBAL - TO MOVE LATER
//-----------------------
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

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
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f,  0.5f,  -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 
    0.5f,  0.5f,  -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, 0.5f,  -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

    -0.5f, -0.5f, 0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 
    0.5f,  -0.5f, 0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 
    0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 
    -0.5f, -0.5f, 0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

    -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 
    -0.5f, 0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 
    -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.0f,1.0f, 0.0f, 
    0.5f,  0.5f,  -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
    0.5f,  -0.5f, -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 
    0.5f,  -0.5f, -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    0.5f,  -0.5f, 0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 
    0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.0f, 1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 
    0.5f,  -0.5f, -0.5f, 0.5f, 0.0f, 0.5f, 1.0f, 1.0f,
    0.5f,  -0.5f, 0.5f,  0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 
    0.5f,  -0.5f, 0.5f,  0.5f, 0.0f, 0.5f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f,  0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 
    -0.5f, -0.5f, -0.5f, 0.5f, 0.0f, 0.5f, 0.0f, 1.0f,

    -0.5f, 0.5f,  -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 
    0.5f,  0.5f,  -0.5f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  0.0f, 0.5f, 0.5f, 1.0f, 0.0f, 
    0.5f,  0.5f,  0.5f,  0.0f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, 0.5f,  0.5f,  0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 
    -0.5f, 0.5f,  -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f};
// clang-format on

glm::vec3 cubePositions[] = {
    glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
    glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processMouseScroll(static_cast<float>(yoffset));
}

void processInput(GLFWwindow* window) {
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
}

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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // SECOND LIGHT
    glGenVertexArrays(amountOfVAOBuffers, &lightSourceVAO);
    glBindVertexArray(lightSourceVAO);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
    //                       (void*)(6 * sizeof(float)));
    // glEnableVertexAttribArray(2);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //DRAW LINES ONLY - FOR DEBUG
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

unsigned int createTexture(const std::string& path, GLenum format) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    const float borderColor[] = {1.0f, 1.0f, 0.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // GL_LINEAR for smoother look from distance
    // GL_NEAREST for more detailed zoom
    //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
    //                  GL_LINEAR_MIPMAP_LINEAR);
    //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    unsigned char* data =
        stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
    return texture;
}

int main() {
    std::cout << "Pioter test!" << std::endl;

    // glfw window creation
    // --------------------
    GLFWwindow* window = createAppWindow();

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    setupVertexBufferData();

    // load and create a texture
    // -------------------------
    stbi_set_flip_vertically_on_load(true);
    unsigned int texture1 = createTexture("./textures/container.jpg", GL_RGB);
    unsigned int texture2 =
        createTexture("./textures/awesomeface.png", GL_RGBA);

    Shader ourShader("shaders/vertex_shader.vs", "shaders/fragment_shader2.fs");
    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);

    Shader lightCubeShader("shaders/vertex_shader.vs",
                           "shaders/fragment_shader1.fs");
    lightCubeShader.use();
    lightCubeShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    lightCubeShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    // MAIN LOOP
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // USER INPUT
        processInput(window);

        // RENDER
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0); // activate the texture unit first before
                                      // binding texture
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        ourShader.use();
        ourShader.setFloat("fadeValue", faceFadeValue);
        ourShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        ourShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

        glm::mat4 projection = glm::perspective(
            glm::radians(camera.getZoom()),
            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);
        glm::mat4 cameraView = camera.getViewMatrix();
        ourShader.setMat4("view", cameraView);
        // render boxes

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
            ourShader.use();
            ourShader.setMat4("model", model);
            glBindVertexArray(vertexArrayObjects);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", cameraView);
        auto lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(0.2f));
        lightCubeShader.setMat4("model", lightModel);
        glBindVertexArray(lightSourceVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse
        // moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // CLEANUP
    glDeleteVertexArrays(amountOfVAOBuffers, &vertexArrayObjects);
    glDeleteBuffers(amountOfVBOBuffers, &vertexBufferObjects);
    glDeleteBuffers(amountOfEBOBuffers, &elementBufferObjects);

    glfwTerminate();
    return 0;
}