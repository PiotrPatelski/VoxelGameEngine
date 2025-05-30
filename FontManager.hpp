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
    unsigned int textureID{}; // ID handle of the glyph texture
    glm::ivec2 size{};        // size of glyph
    glm::ivec2 bearing{};     // Offset from baseline to left/top of glyph
    unsigned int advance{};   // Offset to advance to next glyph
};

class FontManager {
   public:
    FontManager(float screenWidth, float screenHeight);
    FontManager(const FontManager&) = delete;
    FontManager(FontManager&&) = delete;
    FontManager& operator=(const FontManager&) = delete;
    FontManager& operator=(FontManager&&) = delete;
    ~FontManager();
    void renderText(const std::string& text, float x, float y, float scale,
                    const glm::vec3& color);

   private:
    unsigned createTextureForGlyph() const;
    std::pair<char, Character> createCharacter(char charValue) const;
    void initCharacters();
    void initVertexAttributes();
    void initFontTextures();
    void initShader(float screenWidth, float screenHeight);
    void renderCharacter(const Character& character, float x, float y,
                         float scale);
    FT_Library fontLib;
    FT_Face fontFace;
    std::map<char, Character> characters{};
    std::unique_ptr<Shader> shader{nullptr};
    unsigned int vertexArrayObjects{0};
    unsigned int vertexBufferObjects{0};
    const unsigned int amountOfVAOBuffers{1};
    const unsigned int amountOfVBOBuffers{1};
};