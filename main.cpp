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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

float faceFadeValue{0.2};

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    } else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        faceFadeValue += 0.1f;
    } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        faceFadeValue -= 0.1f;
    }
}

float vertices[] = {
    // positions          // colors           // texture coords
    0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
    -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
};
unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

int main() {
    std::cout << "Pioter test!" << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //MAC OS ONLY!

    GLFWwindow* window =
        glfwCreateWindow(800, 600, "OpenGL Pioter Test Window", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    const unsigned int amountOfVBOBuffers{2};
    const unsigned int amountOfVAOBuffers{2};
    // VBO
    unsigned int VBOs[amountOfVBOBuffers];
    glGenBuffers(2, VBOs);
    // VAO
    unsigned int VAOs[amountOfVAOBuffers];
    glGenVertexArrays(2, VAOs);
    // EBO
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    Shader ourShader("shaders/vertex_shader.vs", "shaders/fragment_shader2.fs");

    // ..:: Initialization code :: ..
    // 1. bind Vertex Array Object
    glBindVertexArray(VAOs[0]);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. copy our index array in a element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //DRAW LINES ONLY - FOR DEBUG
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    unsigned int texture1, texture2;
    // TEXTURE 1
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    float borderColor[] = {1.0f, 1.0f, 0.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // GL_LINEAR for smoother look from distance
    // GL_NEAREST for more detailed zoom
    //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
    //                  GL_LINEAR_MIPMAP_LINEAR);
    //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data =
        stbi_load("./textures/container.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);

    // TEXTURE 2
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("./textures/awesomeface.png", &width, &height, &nrChannels,
                     0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);

    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);

    // MATRIX STUFF
    // glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    // glm::mat4 trans = glm::mat4(1.0f);

    // trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
    // // trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0,
    // 0.0, 1.0)); trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    // trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f,
    // 0.0f, 1.0f)); trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));

    // vec = trans * vec;
    // std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")"
    //           << std::endl;

    // unsigned int transformLoc = glGetUniformLocation(ourShader.ID,
    // "transform"); glUniformMatrix4fv(transformLoc, 1, GL_FALSE,
    // glm::value_ptr(trans));

    // MAIN LOOP
    while (!glfwWindowShouldClose(window)) {
        // USER INPUT
        processInput(window);

        // RENDER
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0); // activate the texture unit first before
                                      // binding texture
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        ourShader.use();
        ourShader.setFloat("fadeValue", faceFadeValue);
        glm::mat4 trans = glm::mat4(1.0f);
        // trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
        trans = glm::rotate(trans, (float)glfwGetTime(),
                            glm::vec3(0.0f, 0.0f, 1.0f));

        unsigned int transformLoc =
            glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        glBindVertexArray(VAOs[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        float scaleAmount = std::abs(static_cast<float>(sin(glfwGetTime())));
        trans =
            glm::scale(trans, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        transformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // OUTPUT
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // CLEANUP
    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}