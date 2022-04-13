#version 450

layout(location = 0) in vec3 fragColour;
layout(location = 1) in vec3 fragPosWS;
layout(location = 2) in vec3 fragNormalWS;

layout(location = 0) out vec4 outColour;

struct PointLight {
    vec4 position; //w is unused
    vec4 colour; //w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionMatrix;
    mat4 viewMatrix;
    mat4 inverseViewMatrix;
    vec4 ambientLightColour;
    PointLight pointLights[10];
    int numLights;
} ubo;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

void main() {
    vec3 diffuseLight = ubo.ambientLightColour.rgb * ubo.ambientLightColour.w;
    vec3 specularLight = vec3(0.f);
    const vec3 surfaceNormal = normalize(fragNormalWS);

    vec3 cameraPosWS = ubo.inverseViewMatrix[3].xyz;
    vec3 viewDirection = normalize(cameraPosWS - fragPosWS);

    for (int i = 0; i < ubo.numLights; ++i) {
        PointLight light = ubo.pointLights[i];
        
        vec3 directionToLight = light.position.xyz - fragPosWS.xyz;
        const float attenuation = 1.f / dot(directionToLight, directionToLight); //distance squared
        directionToLight = normalize(directionToLight);

        const float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0.f);
        const vec3 intensity = light.colour.xyz * light.colour.w * attenuation;

        diffuseLight += intensity * cosAngIncidence;

        //specular
        vec3 halfAngle = normalize(directionToLight + viewDirection);
        float blinnTerm = dot(surfaceNormal, halfAngle);
        blinnTerm = clamp(blinnTerm, 0.f, 1.f);
        blinnTerm = pow(blinnTerm, 512.f);
        specularLight += intensity * blinnTerm;
    }
    
    outColour = vec4(diffuseLight * fragColour + specularLight * fragColour, 1.f);
}
