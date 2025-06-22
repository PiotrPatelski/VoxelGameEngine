#version 330 core
in vec2 TexCoord;
in vec3 fragPos;

out vec4 FragColor;

uniform sampler2D entityTexture;
uniform float waterLevelY;
uniform vec3 underwaterTint;
uniform float underwaterMix;

void main() {
    vec3 baseColor = texture(entityTexture, TexCoord).rgb;
    bool isUnderwaterFragment = fragPos.y < waterLevelY + 0.5;
    if (isUnderwaterFragment) {
        baseColor = mix(baseColor, underwaterTint, underwaterMix);
    }
    FragColor = vec4(baseColor, 1.0);
}
