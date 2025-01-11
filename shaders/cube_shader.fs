#version 460 core

out vec4 FragColor;

// in vec3 ourColor;
in vec2 TexCoord;
in vec3 fragPos;
in vec3 normal;
in vec3 lightPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// uniform sampler2D texture2;
uniform float fadeValue;
// uniform vec3 objectColor;
uniform vec3 lightColor;
uniform Material material;
uniform Light light;
uniform float time;

void main()
{
    // FragColor = texture(ourTexture, TexCoord);
    //Ambient light factor
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;

    //Diffuse light factor
    vec3 norm = normalize(normal);
    vec3 lightDirection = normalize (lightPos - fragPos);
    float diff = max(dot(norm, lightDirection), 0.0);
    // vec3 diffuse = light.diffuse * lightColor * (diff * material.diffuse);
    vec3 diffuseMap = texture(material.diffuse, TexCoord).rgb;
    vec3 diffuse = light.diffuse * diff * diffuseMap;

    //Specular light factor
    vec3 viewDirection = normalize(-fragPos);
    // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
    // vec3 viewDirection = normalize(viewPos - fragPos);
    vec3 reflectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
    vec3 specularMap = vec3(texture(material.specular, TexCoord));
    vec3 specular = light.specular * lightColor * spec * specularMap;
    
    //Emission light factor
    // vec3 show = step(vec3(1.0), vec3(1.0) - texture(material.specular, TexCoord).rgb);
    // vec3 emission = texture(material.emission, TexCoord).rgb * show;
    vec2 myTexCoords = TexCoord;
    myTexCoords.x = myTexCoords.x + 0.045f; // slightly shift texture on x for better alignment
    vec3 emissionMap = vec3(texture(material.emission, myTexCoords + vec2(0.0,time*0.75)));
    vec3 emission = emissionMap * (sin(time)*0.5f+0.5f)*2.0;

    // emission mask
    vec3 emissionMask = step(vec3(1.0f), vec3(1.0f)-specularMap);
    emission = emission * emissionMask;

    // vec3 result = (ambient + diffuse + specular) * objectColor;
    vec3 result = ambient + diffuse + specular + emission;
    // FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), fadeValue) * vec4(result, 1.0);
    FragColor = vec4(result, 1.0);
} 