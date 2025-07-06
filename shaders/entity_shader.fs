#version 330 core
in vec2 TexCoord;
in vec3 fragPos;

out vec4 FragColor;

uniform sampler2D entityTexture;
uniform vec3 underwaterTint;
uniform float underwaterMix;
uniform bool isUnderwater;

void main() {
    vec3 baseColor = texture(entityTexture, TexCoord).rgb;
    if (isUnderwater) {
        baseColor = mix(baseColor, underwaterTint, underwaterMix);
    }
    FragColor = vec4(baseColor, 1.0);
}
