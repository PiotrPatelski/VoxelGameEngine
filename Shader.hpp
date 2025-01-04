#pragma once

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
   public:
    // the program ID
    unsigned int ID;

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    // use/activate the shader
    inline void use() { glUseProgram(ID); }
    // utility uniform functions
    inline void setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    inline void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    inline void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

   private:
    void compileShaders(const char* vertexShaderCode,
                        const char* fragmentShaderCode);
};