#version 460 core

out vec4 FragColor;

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

uniform vec3 viewPosition;
uniform DirectionalLight directionalLight;
uniform Material material;
uniform float time;

uniform bool shouldAnimateWater;

uniform sampler3D lightVolume;
uniform vec3 chunkOrigin;
uniform float chunkSize;

vec3 sampleCubeTexture(vec2 texCoord, vec3 normal)
{
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
    const float diff = max(dot(normal, lightDirection), 0.0);
    const vec3 ambient  = light.ambient  * sampleCubeTexture(texCoord, normal);
    const vec3 diffuse  = light.diffuse  * diff * sampleCubeTexture(texCoord, normal);
    const vec3 result = (ambient + diffuse);
    const float skyFactor = clamp((fragPos.y - chunkOrigin.y) / (chunkSize / 4), 0.0, 1.0);
    const vec3 caveAmbient = vec3(0.05);
    return mix(caveAmbient, result, skyFactor);
}

float CalcTorchLight()
{
    const vec3 local = (fragPos - chunkOrigin) / chunkSize;
    const float torchLightIntensity = texture(lightVolume, local).r;
    return torchLightIntensity;
} 

void main()
{
    const vec3 norm = normalize(normal);
    const vec3 viewDirection = normalize(viewPosition - fragPos);
    vec2 outTexCoord = TexCoord;
    if(shouldAnimateWater)
    {
        const float waterSpeed = 0.05;
        outTexCoord += vec2(time * waterSpeed, time * waterSpeed);
    }

    vec3 result = CalcDirectionalLight(directionalLight, norm, viewDirection, outTexCoord);
    result += CalcTorchLight();

    FragColor = vec4(result, material.alpha);
}
