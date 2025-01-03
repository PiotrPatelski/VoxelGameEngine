#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

// float vertices[] = {
//      0.5f,  0.5f, 0.0f,  // top right
//      0.5f,  0.0f, 0.0f,  // bottom right
//     0.0f, 0.0, 0.0f,  // bottom left
//     0.0f,  0.5f, 0.0f,   // top left    
//      1.0f,  0.5f, 0.0f,  // third triangle
// };
// unsigned int indices[] = {  // note that we start from 0!
//     0, 1, 3,   // first triangle
//     1, 2, 3,    // second triangle
//     4, 1, 0
// };

float firstTriangle[] = {
    -0.9f, -0.5f, 0.0f,  // left 
    -0.0f, -0.5f, 0.0f,  // right
    -0.45f, 0.5f, 0.0f,  // top 
};
float secondTriangle[] = {
    0.0f, -0.5f, 0.0f,  // left
    0.9f, -0.5f, 0.0f,  // right
    0.45f, 0.5f, 0.0f   // top 
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

    //VERTEX SHADER
    const auto vertexSourcePath = loadShaderSource("shaders/vertex_shader.vs");
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexSource = vertexSourcePath.c_str();
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    else{
        std::cout << "SHADER::VERTEX::COMPILATION_SUCCESS\n" << infoLog << std::endl;
    }

    //FRAGMENT SHADER
    const auto fragment1SourcePath = loadShaderSource("shaders/fragment_shader1.fs");
    const auto fragment2SourcePath = loadShaderSource("shaders/fragment_shader2.fs");
    unsigned int fragmentShader[2];
    const char* fragmentSource[2];
    unsigned int shaderProgram[2];
    fragmentSource[0] = fragment1SourcePath.c_str();
    fragmentSource[1] = fragment2SourcePath.c_str();

    fragmentShader[0] = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader[0], 1, &fragmentSource[0], NULL);
    glCompileShader(fragmentShader[0]);
    shaderProgram[0] = glCreateProgram();
    glAttachShader(shaderProgram[0], vertexShader);
    glAttachShader(shaderProgram[0], fragmentShader[0]);
    glLinkProgram(shaderProgram[0]);

    glGetProgramiv(shaderProgram[0], GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram[0], 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM0::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    else{
        std::cout << "SHADER::PROGRAM::COMPILATION_SUCCESS\n" << infoLog << std::endl;
    }

    fragmentShader[1] = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader[1], 1, &fragmentSource[1], NULL);
    glCompileShader(fragmentShader[1]);
    shaderProgram[1] = glCreateProgram();
    glAttachShader(shaderProgram[1], vertexShader);
    glAttachShader(shaderProgram[1], fragmentShader[1]);
    glLinkProgram(shaderProgram[1]);

    glGetProgramiv(shaderProgram[1], GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram[1], 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM1::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    else{
        std::cout << "SHADER::PROGRAM::COMPILATION_SUCCESS\n" << infoLog << std::endl;
    }

    glUseProgram(shaderProgram[1]);
    glUseProgram(shaderProgram[0]);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader[0]);
    glDeleteShader(fragmentShader[1]);

    //DRAW SHADERS?
    // ..:: Initialization code :: ..
    //FIRST TRIANGLE
    // 1. bind Vertex Array Object
    glBindVertexArray(VAOs[0]);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle, GL_STATIC_DRAW);
    // 3. copy our index array in a element buffer for OpenGL to use
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //SECOND TRIANGLE
    glBindVertexArray(VAOs[1]);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);
    // 3. copy our index array in a element buffer for OpenGL to use
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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
        // 3. use our shader program when we want to render an object
        glUseProgram(shaderProgram[0]);
        glBindVertexArray(VAOs[0]);
        // 4. now draw the object 
        // glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glUseProgram(shaderProgram[1]);
        glBindVertexArray(VAOs[1]);
        // 4. now draw the object 
        // glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        //OUTPUT
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glfwTerminate();
    return 0;
}