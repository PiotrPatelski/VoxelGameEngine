#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace Steve {
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

const glm::vec3 headScale{0.5f, 0.5f, 0.5f};
const glm::vec3 bodyScale{0.5f, 0.75f, 0.25f};
const glm::vec3 armScale{0.25f, 0.75f, 0.25f};
const glm::vec3 legScale{0.25f, 0.75f, 0.25f};
const glm::vec3 headOffset{0.0f, 1.625f, 0.0f};
const glm::vec3 bodyOffset{0.0f, 1.0f, 0.0f};
const glm::vec3 leftArmOffset{-0.375f, 1.0f, 0.0f};
const glm::vec3 rightArmOffset{0.375f, 1.0f, 0.0f};
const glm::vec3 leftLegOffset{-0.125f, 0.25f, 0.0f};
const glm::vec3 rightLegOffset{0.125f, 0.25f, 0.0f};
const glm::vec3 hitboxOffset{0.f, 0.875f, 0.f};
const glm::vec3 hitboxScale{1.f, 2.0f, 0.5f};
const glm::vec3 centerPointToTopRotationAxisOffset{0.0f, 0.5f, 0.0f};
const glm::vec3 topPointToCenterRotationAxisOffset{0.0f, -0.5f, 0.0f};

} // namespace Steve