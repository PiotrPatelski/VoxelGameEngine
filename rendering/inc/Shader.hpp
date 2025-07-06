#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
   public:
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();
    Shader(const Shader&) = delete;
    Shader(Shader&&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&&) = delete;
    // use/activate the shader
    inline void use() { glUseProgram(programID); }

    inline void setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
    }
    inline void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
    }
    inline void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
    }
    inline void setMat4(const std::string& name, const glm::mat4& value) const {
        glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1,
                           GL_FALSE, glm::value_ptr(value));
    }
    inline void setVec3(const std::string& name, float x, float y,
                        float z) const {
        glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1,
                     glm::value_ptr(glm::vec3(x, y, z)));
    }
    inline void setVec3(const std::string& name, const glm::vec3& vec) const {
        glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1,
                     glm::value_ptr(vec));
    }

   private:
    void compileShaders(const char* vertexShaderCode,
                        const char* fragmentShaderCode);
    unsigned int programID{};
};