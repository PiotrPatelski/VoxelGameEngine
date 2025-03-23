
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;
layout (location = 4) in mat4 instanceMatrix;

out vec3 ourColor;
out vec2 TexCoord;
out vec3 fragPos;
out vec3 normal;

uniform vec3 lightPosition;

// uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    ourColor = aColor;
    TexCoord = aTexCoord;
    // fragPos = vec3(model * vec4(aPos, 1.0));
    // normal = mat3(transpose(inverse(model))) * aNormal;
    // // note that we read the multiplication from right to left
    // gl_Position = projection * view * vec4(fragPos, 1.0);

    fragPos = vec3(instanceMatrix * vec4(aPos, 1.0));
    normal = mat3(transpose(inverse(instanceMatrix))) * aNormal;
    // note that we read the multiplication from right to left
    gl_Position = projection * view * instanceMatrix * vec4(fragPos, 1.0);

    // Inversing matrices is a costly operation for shaders, 
    //so wherever possible try to avoid doing inverse operations 
    //since they have to be done on each vertex of your scene. 
    //For learning purposes this is fine, but for an efficient application 
    //you'll likely want to calculate the normal matrix on the CPU 
    //and send it to the shaders via a uniform before drawing (just like the model matrix). 
    // lightPos = vec3(view * vec4(lightPosition, 1.0));
}