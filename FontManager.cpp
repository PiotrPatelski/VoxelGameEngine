#include "FontManager.hpp"
#include <iostream>
#include <array>

namespace {
std::array<std::array<float, 4>, 6> buildGlyphVertices(const Character& ch,
                                                       float x, float y,
                                                       float scale) {
    const auto xpos = x + static_cast<float>(ch.bearing.x) * scale;
    const auto ypos = y - static_cast<float>(ch.size.y - ch.bearing.y) * scale;
    const auto width = static_cast<float>(ch.size.x) * scale;
    const auto height = static_cast<float>(ch.size.y) * scale;

    return {{{{xpos, ypos + height, 0.0f, 0.0f}},
             {{xpos, ypos, 0.0f, 1.0f}},
             {{xpos + width, ypos, 1.0f, 1.0f}},

             {{xpos, ypos + height, 0.0f, 0.0f}},
             {{xpos + width, ypos, 1.0f, 1.0f}},
             {{xpos + width, ypos + height, 1.0f, 0.0f}}}};
}
} // namespace

FontManager::FontManager(float screenWidth, float screenHeight) {
    std::cout << "FontManager::Init!" << std::endl;
    if (FT_Init_FreeType(&fontLib)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library"
                  << std::endl;
        throw;
    }

    if (FT_New_Face(fontLib, "fonts/Raleway-Regular.ttf", 0, &fontFace)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        throw;
    }

    FT_Set_Pixel_Sizes(fontFace, 0, 48);

    if (FT_Load_Char(fontFace, 'X', FT_LOAD_RENDER)) {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        throw;
    }

    initFontTextures();
    initShader(screenWidth, screenHeight);
}

FontManager::~FontManager() {
    std::cout << "FontManager::Shutdown!" << std::endl;
    if (vertexArrayObjects) {
        glDeleteBuffers(amountOfVAOBuffers, &vertexArrayObjects);
    }
    if (vertexBufferObjects) {
        glDeleteVertexArrays(amountOfVBOBuffers, &vertexBufferObjects);
    }
}

unsigned FontManager::createTextureForGlyph() const {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, fontFace->glyph->bitmap.width,
                 fontFace->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                 fontFace->glyph->bitmap.buffer);
    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return texture;
}

std::pair<char, Character> FontManager::createCharacter(char charValue) const {
    const glm::ivec2 characterSize{fontFace->glyph->bitmap.width,
                                   fontFace->glyph->bitmap.rows};
    const glm::ivec2 characterBearing{fontFace->glyph->bitmap_left,
                                      fontFace->glyph->bitmap_top};
    const auto characterAdvance =
        static_cast<unsigned int>(fontFace->glyph->advance.x);
    Character character = {createTextureForGlyph(), characterSize,
                           characterBearing, characterAdvance};
    return std::pair<char, Character>(charValue, character);
}

void FontManager::initCharacters() {
    for (unsigned char c = 0; c < 128; c++) {
        // load character glyph
        if (FT_Load_Char(fontFace, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        characters.insert(createCharacter(c));
    }
}

void FontManager::initVertexAttributes() {
    // configure VAO/VBO for texture quads
    // -----------------------------------
    const unsigned int stride = 4 * sizeof(float);
    glGenVertexArrays(amountOfVAOBuffers, &vertexArrayObjects);
    glGenBuffers(amountOfVBOBuffers, &vertexBufferObjects);
    glBindVertexArray(vertexArrayObjects);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects);
    glBindVertexArray(vertexArrayObjects);
}

void FontManager::initFontTextures() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    initCharacters();
    // destroy FreeType once we're finished
    FT_Done_Face(fontFace);
    FT_Done_FreeType(fontLib);

    initVertexAttributes();
}

void FontManager::initShader(float screenWidth, float screenHeight) {
    shader = std::make_unique<Shader>("shaders/font_shader.vs",
                                      "shaders/font_shader.fs");
    const glm::mat4 projection =
        glm::ortho(0.0f, static_cast<float>(screenWidth), 0.0f,
                   static_cast<float>(screenHeight));
    shader->use();
    shader->setMat4("projection", projection);
}

void FontManager::renderCharacter(const Character& character, float x, float y,
                                  float scale) {
    const auto vertices = buildGlyphVertices(character, x, y, scale);

    glBindTexture(GL_TEXTURE_2D, character.textureID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void FontManager::renderText(const std::string& text, float x, float y,
                             float scale, const glm::vec3& color) {
    // activate corresponding render state
    shader->use();
    shader->setVec3("textColor", color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vertexArrayObjects);

    // iterate through all characters
    for (const auto& character : text) {
        const auto ch = characters[character];

        renderCharacter(ch, x, y, scale);
        // now advance cursors for next glyph (note that advance is number of
        // 1/64 pixels)
        x += static_cast<float>(ch.advance >> 6) *
             scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide
                    // amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(vertexArrayObjects);
    glBindTexture(GL_TEXTURE_2D, 0);
}
