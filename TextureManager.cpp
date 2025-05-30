#include "TextureManager.hpp"
#include <iostream>
#include <stb_image.h>

std::unordered_map<std::string, unsigned int> TextureManager::textures;

TextureManager::TextureManager() {
    std::cout << "TextureManager::Init!" << std::endl;
    stbi_set_flip_vertically_on_load(true);
}

unsigned int setupTextureFromFileData(unsigned char *data, int width,
                                      int height, int nrChannels) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLenum format = GL_RGB;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    const float borderColor[] = {1.0f, 1.0f, 0.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return texture;
}

unsigned int TextureManager::loadTextureFromFile(const std::string &filePath) {
    if (textures.find(filePath) != textures.end()) return textures[filePath];

    int width, height, nrChannels;
    unsigned char *data =
        stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        std::cout << "Failed to load texture: " << filePath << std::endl;
        return 0;
    }
    const auto textureId =
        setupTextureFromFileData(data, width, height, nrChannels);
    textures[filePath] = textureId;
    std::cout << "Loaded texture: " << filePath << " with ID: " << textureId
              << std::endl;

    return textureId;
}

unsigned int TextureManager::GetTexture(const std::string &filePath) {
    if (textures.find(filePath) != textures.end()) return textures[filePath];
    return loadTextureFromFile(filePath);
}

unsigned int TextureManager::BindTextureToUnit(const std::string &filePath,
                                               unsigned int unit) {
    unsigned int texID = GetTexture(filePath);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texID);
    return texID;
}

void TextureManager::Clear() {
    for (auto &pair : textures) {
        glDeleteTextures(1, &pair.second);
    }
    textures.clear();
}

TextureManager::~TextureManager() {
    std::cout << "TextureManager::Shutdown!" << std::endl;
}