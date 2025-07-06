#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 fragPos;
out vec3 normal;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoord = aTexCoord;
    fragPos = aPos;
    normal = aNormal;
    gl_Position = projection * view * vec4(aPos, 1.0);
}
