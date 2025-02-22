#version 460 core

out vec4 FragColor;

// in vec3 ourColor;
in vec2 TexCoord;
in vec3 fragPos;
in vec3 normal;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    // sampler2D emission;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NR_POINT_LIGHTS 4

struct SpotLight {    
    vec3 position;
    vec3 direction;
    float innerCutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

// uniform sampler2D texture2;
uniform vec3 viewPosition;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform float fadeValue;
// uniform vec3 objectColor;
// uniform vec3 lightColor;
uniform Material material;
uniform float time;

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection)
{
    vec3 lightDirection = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDirection), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * texture(material.diffuse, TexCoord).rgb;
    vec3 diffuse  = light.diffuse  * diff * texture(material.diffuse, TexCoord).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, TexCoord).rgb;
    return (ambient + diffuse + specular);
} 


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDirection)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
} 

void main()
{
    // FragColor = texture(ourTexture, TexCoord);
    // vec3 lightDirection = normalize(light.spotlightPosition - fragPos);
    // vec3 lightDirection = normalize(-light.direction);

    vec3 norm = normalize(normal);
    vec3 viewDirection = normalize(viewPosition - fragPos);
    
    //Emission light factor
    // vec2 myTexCoords = TexCoord;
    // myTexCoords.x = myTexCoords.x + 0.045f; // slightly shift texture on x for better alignment
    // vec3 emissionMap = vec3(texture(material.emission, myTexCoords + vec2(0.0,time*0.75)));
    // vec3 emission = emissionMap * (sin(time)*0.5f+0.5f)*2.0;

    //emission mask
    // vec3 emissionMask = step(vec3(1.0f), vec3(1.0f)-texture(material.specular, TexCoord).rgb);
    // emission = emission * emissionMask;
    
    // phase 1: Directional lighting
    vec3 result = CalcDirectionalLight(directionalLight, norm, viewDirection);
    // phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, fragPos, viewDirection);    
    // phase 3: Spot light
    result += CalcSpotLight(spotLight, norm, fragPos, viewDirection);
    // FragColor = vec4(result + emission, 1.0);
    FragColor = vec4(result, 1.0);
 
} 