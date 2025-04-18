#version 460 core

out vec4 FragColor;

// in vec3 ourColor;
in vec2 TexCoord;
in vec3 fragPos;
in vec3 normal;

struct Material {
    sampler2D diffuseMain;
    sampler2D diffuseSecondary;
    float shininess;
    float alpha;
    bool useSecondaryTexture;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
};

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
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
};  

uniform vec3 viewPosition;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform float fadeValue;
uniform Material material;
uniform float time;

uniform bool isUnderwater;
uniform vec3 underwaterTint;
uniform float underwaterMix; // Blend factor: 0.0 means no tint, 1.0 means full tint.
uniform bool shouldAnimateWater;

vec3 sampleCubeTexture(vec2 texCoord, vec3 normal)
{
    // If the face's normal has a high absolute Y component, consider it top/bottom.
    const bool isFaceTopOrBottom = abs(normal.y) >= 0.5;
    if(material.useSecondaryTexture && isFaceTopOrBottom)
    {
        return texture(material.diffuseSecondary, texCoord).rgb;
    }
    return texture(material.diffuseMain, texCoord).rgb;
}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection, vec2 texCoord)
{
    const vec3 lightDirection = normalize(-light.direction);
    // diffuse shading
    const float diff = max(dot(normal, lightDirection), 0.0);
    // combine results
    const vec3 ambient  = light.ambient  * sampleCubeTexture(texCoord, normal);
    const vec3 diffuse  = light.diffuse  * diff * sampleCubeTexture(texCoord, normal);
    return (ambient + diffuse);
} 


// vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection, vec2 texCoord)
// {
//     const vec3 lightDir = normalize(light.position - fragPos);
//     // diffuse shading
//     const float diff = max(dot(normal, lightDir), 0.0);
//     // attenuation
//     const float distance = length(light.position - fragPos);
//     const float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
//     // combine results
//     vec3 ambient = light.ambient * sampleCubeTexture(texCoord, normal);
//     vec3 diffuse = light.diffuse * diff * sampleCubeTexture(texCoord, normal);
//     ambient *= attenuation;
//     diffuse *= attenuation;
//     return (ambient + diffuse);
// }

// vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDirection, vec2 texCoord)
// {
//     const vec3 lightDir = normalize(light.position - fragPos);
//     // diffuse shading
//     const float diff = max(dot(normal, lightDir), 0.0);
//     // attenuation
//     const float distance = length(light.position - fragPos);
//     const float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
//     // spotlight intensity
//     const float theta = dot(lightDir, normalize(-light.direction)); 
//     const float epsilon = light.innerCutOff - light.outerCutOff;
//     const float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
//     // combine results
//     vec3 ambient = light.ambient * sampleCubeTexture(texCoord, normal);
//     vec3 diffuse = light.diffuse * diff * sampleCubeTexture(texCoord, normal);
//     ambient *= attenuation * intensity;
//     diffuse *= attenuation * intensity;

//     return (ambient + diffuse);
// } 

void main()
{
    const vec3 norm = normalize(normal);
    const vec3 viewDirection = normalize(viewPosition - fragPos);
    vec2 outTexCoord = TexCoord;
    if(shouldAnimateWater)
    {
        // A simple scrolling effect.
        const float waterSpeed = 0.05;
        outTexCoord += vec2(time * waterSpeed, time * waterSpeed);
    }
    // phase 1: Directional lighting
    vec3 result = CalcDirectionalLight(directionalLight, norm, viewDirection, outTexCoord);
    // // phase 2: Point lights
    // for(int i = 0; i < NR_POINT_LIGHTS; i++)
    //     result += CalcPointLight(pointLights[i], norm, fragPos, viewDirection, outTexCoord);
    // // phase 3: Spot light
    // result += CalcSpotLight(spotLight, norm, fragPos, viewDirection, outTexCoord);
    // FragColor = vec4(result + emission, 1.0);
    if (isUnderwater) {
        result = mix(result, underwaterTint, underwaterMix);
    }
    FragColor = vec4(result, material.alpha);
 
} 