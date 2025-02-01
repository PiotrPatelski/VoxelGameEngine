#pragma once
#include <map>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Shader.hpp"

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Offset to advance to next glyph
};

class FontManager {
   public:
    FontManager(float screenWidth, float screenHeight);
    ~FontManager();
    void renderText(const std::string& text, float x, float y, float scale,
                    const glm::vec3& color);

   private:
    void initFontTextures();
    void initShader(float screenWidth, float screenHeight);
    FT_Library fontLib;
    FT_Face fontFace;
    std::map<char, Character> characters{};
    std::unique_ptr<Shader> shader{nullptr};
    unsigned int vertexArrayObjects{0};
    unsigned int vertexBufferObjects{0};
    const unsigned int amountOfVAOBuffers{1};
    const unsigned int amountOfVBOBuffers{1};
};