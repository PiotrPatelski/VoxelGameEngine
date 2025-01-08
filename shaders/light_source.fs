#version 460 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float fadeValue;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
    // FragColor = texture(ourTexture, TexCoord);
    // float ambientStrength = 0.1;
    // vec3 ambient = ambientStrength * lightColor;

    // vec3 result = ambient * objectColor;
    // FragColor = vec4(result, 1.0);
    FragColor = vec4(objectColor, 1.0);
} 