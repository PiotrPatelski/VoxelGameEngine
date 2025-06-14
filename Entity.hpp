#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "Shader.hpp"
#include "VertexData.hpp"
#include "TextureManager.hpp"
#include "BodyPart.hpp"
#include "Hitbox.hpp"

// clang-format off
//TexCord:{X, Y}
//BACK
//FRONT
//LEFT
//RIGHT
//BOTTOM
//TOP
const std::vector<glm::vec2> headTexCoords = {
    
    {0.5f, 0.25f}, 
    {0.375f, 0.125f}, 
    {0.375f, 0.25f}, 
    {0.5f, 0.125f},
    
    {0.25f, 0.25f},
    {0.125f, 0.25f},
    {0.125f, 0.125f},
    {0.25f, 0.125f},
   
    {0.25f, 0.125f},
    {0.375f, 0.125f},
    {0.375f, 0.25f},
    {0.25f, 0.25f},
    
    {0.25f, 0.125f},
    {0.375f, 0.25f},
    {0.375f, 0.125f},
    {0.25f, 0.25f},
    
    {0.25f, 0.0f},
    {0.375f, 0.0f},
    {0.375f, 0.125f},
    {0.25f, 0.125f},

    {0.125f, 0.125f},
    {0.25f, 0.0f},
    {0.25f, 0.125f},
    {0.125f, 0.0f}};

const std::vector<glm::vec2> bodyTexCoords = {
    {0.625f, 0.5f}, 
    {0.5f, 0.3125f}, 
    {0.5f, 0.5f}, 
    {0.625f, 0.3125f},

     {0.4375f, 0.5f},
     {0.3125f, 0.5f},
     {0.3125f, 0.3125f},
     {0.4375f, 0.3125f},

     {0.4375f, 0.3125f},
     {0.5f, 0.3125f},
     {0.5f, 0.5f},
     {0.4375f, 0.5f},

     {0.25f, 0.3125f},
     {0.3125f, 0.5f},
     {0.3125f, 0.3125f},
     {0.25f, 0.5f},

     {0.4375f, 0.25f},
     {0.5625f, 0.25f},
     {0.5625f, 0.3125f},
     {0.4375f, 0.3125f},

     {0.3125f, 0.3125f},
     {0.4375f, 0.25f},
     {0.4375f, 0.3125f},
     {0.3125f, 0.25f}};

const std::vector<glm::vec2> leftArmTexCoords = {
    {0.75f, 0.5f}, 
    {0.6875f, 0.3125f}, 
    {0.6875f, 0.5f}, 
    {0.75f, 0.3125f},

    {0.75f, 0.5f}, 
    {0.6875f, 0.5f}, 
    {0.6875f, 0.3125f}, 
    {0.75f, 0.3125f},

    {0.6875f, 0.3125f}, 
    {0.75f, 0.3125f}, 
    {0.75f, 0.5f}, 
    {0.6875f, 0.5f},
    
    {0.625f, 0.3125f}, 
    {0.6875f, 0.5f}, 
    {0.6875f, 0.3125f}, 
    {0.625f, 0.5f},

    {0.75f, 0.25f}, 
    {0.8125f, 0.25f}, 
    {0.8125f, 0.3125f}, 
    {0.75f, 0.3125f},

    {0.6875f, 0.3125f}, 
    {0.75f, 0.25f}, 
    {0.75f, 0.3125f}, 
    {0.6875f, 0.25f}
};

const std::vector<glm::vec2> rightArmTexCoords = {
    {0.75f, 0.5f}, 
    {0.6875f, 0.3125f}, 
    {0.6875f, 0.5f}, 
    {0.75f, 0.3125f},

    {0.75f, 0.5f}, 
    {0.6875f, 0.5f}, 
    {0.6875f, 0.3125f}, 
    {0.75f, 0.3125f},

    {0.625f, 0.3125f}, 
    {0.6875f, 0.3125f}, 
    {0.6875f, 0.5f}, 
    {0.625f, 0.5f},

    {0.6875f, 0.3125f}, 
    {0.75f, 0.5f}, 
    {0.75f, 0.3125f}, 
    {0.6875f, 0.5f},

    {0.75f, 0.25f}, 
    {0.8125f, 0.25f}, 
    {0.8125f, 0.3125f}, 
    {0.75f, 0.3125f},

    {0.6875f, 0.3125f}, 
    {0.75f, 0.25f}, 
    {0.75f, 0.3125f}, 
    {0.6875f, 0.25f}
};

const std::vector<glm::vec2> leftLegTexCoords = {
    {0.25f, 0.5f}, 
    {0.1875f, 0.3125f}, 
    {0.1875f, 0.5f}, 
    {0.25f, 0.3125f},

    {0.125f, 0.5f}, 
    {0.0625f, 0.5f}, 
    {0.0625f, 0.3125f}, 
    {0.125f, 0.3125f},

    {0.125f, 0.3125f}, 
    {0.1875f, 0.3125f}, 
    {0.1875f, 0.5f}, 
    {0.125f, 0.5f},

    {0.1875f, 0.3125f}, 
    {0.25f, 0.5f}, 
    {0.25f, 0.3125f}, 
    {0.1875f, 0.5f},

    {0.125f, 0.25f}, 
    {0.1725f, 0.25f}, 
    {0.1725f, 0.3125f}, 
    {0.125f, 0.3125f},

    {0.0625f, 0.3125f}, 
    {0.1100f, 0.25f}, 
    {0.1100f, 0.3125f}, 
    {0.0625f, 0.25f}
};

const std::vector<glm::vec2> rightLegTexCoords = {
    {0.25f, 0.5f}, 
    {0.1875f, 0.3125f}, 
    {0.1875f, 0.5f}, 
    {0.25f, 0.3125f},

    {0.125f, 0.5f}, 
    {0.0625f, 0.5f}, 
    {0.0625f, 0.3125f}, 
    {0.125f, 0.3125f},

    {0.125f, 0.3125f}, 
    {0.1875f, 0.3125f}, 
    {0.1875f, 0.5f}, 
    {0.125f, 0.5f},

    {0.125f, 0.3125f}, 
    {0.1875f, 0.5f}, 
    {0.1875f, 0.3125f}, 
    {0.125f, 0.5f},

    {0.125f, 0.25f}, 
    {0.1725f, 0.25f}, 
    {0.1725f, 0.3125f}, 
    {0.125f, 0.3125f},

    {0.0625f, 0.3125f}, 
    {0.1100f, 0.25f}, 
    {0.1100f, 0.3125f}, 
    {0.0625f, 0.25f}
};

// clang-format on
class Entity {
   public:
    Entity();
    ~Entity();

    void render();
    void update(const glm::mat4& view, const glm::mat4& projection);

    glm::vec3 getPosition() const { return entityPosition; }

   private:
    void updateShaders(const glm::mat4& view, const glm::mat4& projection);
    void updateMoveAnimation();
    glm::mat4 createLimbTransform(const glm::vec3& offset, float angle,
                                  const glm::vec3& rotationAxis) const;
    std::unique_ptr<BodyPart> head{nullptr};
    std::unique_ptr<BodyPart> body{nullptr};
    std::unique_ptr<BodyPart> leftArm{nullptr};
    std::unique_ptr<BodyPart> rightArm{nullptr};
    std::unique_ptr<BodyPart> leftLeg{nullptr};
    std::unique_ptr<BodyPart> rightLeg{nullptr};
    std::unique_ptr<Hitbox> hitbox{nullptr};
    GLuint elementBufferObject{0};
    glm::vec3 entityPosition{0.0f, 0.0f, 0.0f};
    GLuint textureID{0};
    std::unique_ptr<Shader> shader{nullptr};
    glm::mat4 headTransform{};
    glm::mat4 bodyTransform{};
    glm::mat4 leftArmTransform{};
    glm::mat4 rightArmTransform{};
    glm::mat4 leftLegTransform{};
    glm::mat4 rightLegTransform{};
};
