#include "FontManager.hpp"
#include <iostream>

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
}

void FontManager::initFontTextures() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++) {
        // load character glyph
        if (FT_Load_Char(fontFace, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
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
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(fontFace->glyph->bitmap.width,
                       fontFace->glyph->bitmap.rows),
            glm::ivec2(fontFace->glyph->bitmap_left,
                       fontFace->glyph->bitmap_top),
            static_cast<unsigned int>(fontFace->glyph->advance.x)};
        characters.insert(std::pair<char, Character>(c, character));
    }
    // destroy FreeType once we're finished
    FT_Done_Face(fontFace);
    FT_Done_FreeType(fontLib);

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

void FontManager::initShader(float screenWidth, float screenHeight) {
    shader = std::make_unique<Shader>("shaders/font_shader.vs",
                                      "shaders/font_shader.fs");
    const glm::mat4 projection =
        glm::ortho(0.0f, static_cast<float>(screenWidth), 0.0f,
                   static_cast<float>(screenHeight));
    shader->use();
    shader->setMat4("projection", projection);
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
        Character ch = characters[character];

        float xpos = x + static_cast<float>(ch.Bearing.x) * scale;
        float ypos = y - static_cast<float>(ch.Size.y - ch.Bearing.y) * scale;

        float w = static_cast<float>(ch.Size.x) * scale;
        float h = static_cast<float>(ch.Size.y) * scale;
        // update VBO for each character
        float vertices[6][4] = {
            {xpos, ypos + h, 0.0f, 0.0f},    {xpos, ypos, 0.0f, 1.0f},
            {xpos + w, ypos, 1.0f, 1.0f},

            {xpos, ypos + h, 0.0f, 0.0f},    {xpos + w, ypos, 1.0f, 1.0f},
            {xpos + w, ypos + h, 1.0f, 0.0f}};
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects);
        glBufferSubData(
            GL_ARRAY_BUFFER, 0, sizeof(vertices),
            vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of
        // 1/64 pixels)
        x += static_cast<float>(ch.Advance >> 6) *
             scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide
                    // amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(vertexArrayObjects);
    glBindTexture(GL_TEXTURE_2D, 0);
}
