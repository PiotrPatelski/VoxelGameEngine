#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.hpp"

// Function to load shader source code from a file
std::string loadShaderSource(const char* filepath)
{
    std::ifstream shaderFile(filepath); // Open the file
    if (!shaderFile) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCESSFULLY_READ: " << filepath << std::endl;
        return "";
    }
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf(); // Read the file contents into the stream
    return shaderStream.str(); // Return the file contents as a string
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

float firstTriangle[] = {
    // positions         // colors
     0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
     1.0f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
     0.5f,  -0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
};
float secondTriangle[] = {
    // positions         // colors
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
};

int main()
{
    std::cout<<"Pioter test!"<<std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //MAC OS ONLY!

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Pioter Test Window", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    const unsigned int amountOfVBOBuffers{2};
    const unsigned int amountOfVAOBuffers{2};
    //VBO
    unsigned int VBOs[amountOfVBOBuffers];
    glGenBuffers(2, VBOs);
    //VAO
    unsigned int VAOs[amountOfVAOBuffers];
    glGenVertexArrays(2, VAOs);
    //EBO
    unsigned int EBO;
    glGenBuffers(1, &EBO);



    Shader ourShader("shaders/vertex_shader.vs", "shaders/fragment_shader2.fs");

    // ..:: Initialization code :: ..
    // 1. bind Vertex Array Object
    glBindVertexArray(VAOs[0]);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle, GL_STATIC_DRAW);
    // 3. copy our index array in a element buffer for OpenGL to use
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    //SECOND TRIANGLE
    glBindVertexArray(VAOs[1]);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);
    // 3. copy our index array in a element buffer for OpenGL to use
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //DRAW LINES ONLY - FOR DEBUG
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //MAIN LOOP
    while(!glfwWindowShouldClose(window))
    {
        //USER INPUT
        processInput(window);

        //RENDER
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ourShader.use();
        ourShader.setFloat("horizontalOffset", 0.3f);
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        //OUTPUT
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    //CLEANUP
    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);

    glfwTerminate();
    return 0;
}