#version 460 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in vec3 fragPos;
in vec3 normal;
in vec3 lightPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float fadeValue;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform Material material;
uniform Light light;

void main()
{
    // FragColor = texture(ourTexture, TexCoord);
    //Ambient light factor
    vec3 ambient = light.ambient * material.ambient;

    //Diffuse light factor
    vec3 norm = normalize(normal);
    vec3 lightDirection = normalize (lightPos - fragPos);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = light.diffuse * lightColor * (diff * material.diffuse);

    //Specular light factor
    vec3 viewDirection = normalize(-fragPos);
    // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
    // vec3 viewDirection = normalize(viewPos - fragPos);
    vec3 reflectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
    vec3 specular = light.specular * lightColor * (spec * material.specular);
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), fadeValue) * vec4(result, 1.0);
    // FragColor = vec4(lightColor * objectColor, 1.0);
    // FragColor = vec4(result, 1.0);
} 