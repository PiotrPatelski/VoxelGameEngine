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
    FragColor = vec4(1.0);
} 