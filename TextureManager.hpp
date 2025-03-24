#pragma once

#include <string>
#include <unordered_map>
#include <glad/glad.h>

class TextureManager {
   public:
    TextureManager();
    static unsigned int loadTextureFromFile(const std::string &filePath);
    static unsigned int GetTexture(const std::string &filePath);
    static unsigned int BindTextureToUnit(const std::string &filePath,
                                          unsigned int unit);
    static void Clear();

   private:
    static std::unordered_map<std::string, unsigned int> textures;
};