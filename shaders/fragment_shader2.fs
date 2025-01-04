#version 460 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float fadeValue;

void main()
{
    // FragColor = texture(ourTexture, TexCoord);
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), fadeValue);
    // FragColor = vec4(ourColor, 1.0);
} 