#version 330 core

const int NUM_CASCADES = 3;     // Number of cascaded shadows

// Light information
uniform vec3 lightIntensity;

// Material information
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float shininess;
uniform float alpha;

// Texture sampler
uniform sampler2D diffuseSampler;
uniform sampler2D normalSampler;
uniform sampler2D depthSampler;
uniform sampler2D shadowMap[NUM_CASCADES];

// Far plane distance of each shadow cascade
uniform float endCascade[NUM_CASCADES];

// Amplitude of parallax effect in bump mapping
uniform float heightScale;

in vec2 texCoord;

in Proj {
    highp float z;
} proj;

in LightProj {
    highp vec4 position[NUM_CASCADES];
} lightProj;

in Tangent {
    highp vec3 lightDir;
    highp vec3 fragPos;
} tangent;

out vec4 fragColor;

// Texel size for PCF
#define PCF_TEXELSIZE_FILTER 2
// Show the area for each shadow map
// #define CSM_DEBUG



// Check if fragment is in shadow or not and return shadow coefficient
float shadowCalculation(
    int cascadeIndex, vec4 fragPosLightSpace, vec3 normal, vec3 lightDir
) {
    // Perform perspective divide and transform to [0,1] range
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective
//     float closestDepth = texture(shadowMap[cascadeIndex], projCoords.xy).r; 
    float closestDepth;
    switch(cascadeIndex) {
        case 0: closestDepth = texture(shadowMap[0], projCoords.xy).r; break;
        case 1: closestDepth = texture(shadowMap[1], projCoords.xy).r; break;
        case 2: closestDepth = texture(shadowMap[2], projCoords.xy).r; break;
    }
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check if the current fragment is in the shadow
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.0005);
    // Use PCF
    float shadow = 0.0;
//     vec2 texelSize = 1.0 / textureSize(shadowMap[cascadeIndex], 0);
    vec2 texelSize;
    switch(cascadeIndex) {
        case 0: texelSize = 1.0 / textureSize(shadowMap[0], 0); break;
        case 1: texelSize = 1.0 / textureSize(shadowMap[1], 0); break;
        case 2: texelSize = 1.0 / textureSize(shadowMap[2], 0); break;
    }
    for(int x = -PCF_TEXELSIZE_FILTER; x <= PCF_TEXELSIZE_FILTER; ++x)
    {
        for(int y = -PCF_TEXELSIZE_FILTER; y <= PCF_TEXELSIZE_FILTER; ++y)
        {
//             float pcfDepth = texture(
//                 shadowMap[cascadeIndex], projCoords.xy + vec2(x, y) * texelSize
//             ).r; 
            float pcfDepth;
            switch(cascadeIndex) {
                case 0: pcfDepth = texture(shadowMap[0], projCoords.xy + vec2(x, y) * texelSize).r; break;
                case 1: pcfDepth = texture(shadowMap[1], projCoords.xy + vec2(x, y) * texelSize).r; break;
                case 2: pcfDepth = texture(shadowMap[2], projCoords.xy + vec2(x, y) * texelSize).r; break;
            }
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    float normalizePCF = (2.0*PCF_TEXELSIZE_FILTER + 1.0) * 
        (2.0*PCF_TEXELSIZE_FILTER + 1.0);
    shadow /= normalizePCF;
    
    // Force no shadow when outside of the shadow map
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}



/**
 * Compute the displaced texture coordinates for bump mapping.
 */
vec2 parallaxMapping(vec2 texCoords, vec3 viewDir) { 
    // Number of depth layers
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // Calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // Depth of current layer
    float currentLayerDepth = 0.0;
    // The amount to shift the texture coordinates per layer (from vector P)
    vec2 P = vec2(viewDir.x, -viewDir.y) * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
    // Get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(depthSampler, currentTexCoords).r;
    
    while(currentLayerDepth < currentDepthMapValue)
    {
        // Shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // Get depthmap value at current texture coordinates
        currentDepthMapValue = texture(depthSampler, currentTexCoords).r;  
        // Get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // Get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // Get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = 
        texture(depthSampler, prevTexCoords).r 
        - currentLayerDepth + layerDepth;
    
    // Interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = mix(currentTexCoords, prevTexCoords, weight);

    return finalTexCoords;
}



void main() {
    // Calculate light direction
    vec3 s = normalize(-tangent.lightDir);

    // Calculate the vector from the fragment to eye position (view direction)
    vec3 v = normalize(-tangent.fragPos);
    
    // Compute the halfway vector for the Blinn-Phong lighting model
    vec3 h = normalize(s + v);
    
    // Offset texture coordinates with bump mapping
    vec2 texCoordOffset = parallaxMapping(texCoord, v);
    
    vec3 normal = texture(normalSampler, texCoordOffset).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    //normal = vec3(0.0, 0.0, 1.0);
    
    // Calculate the diffuse contribution
    vec3 diffuse = vec3(max(dot(s, normal), 0.0));

    // Calculate specular contribution (Blinn-Phong model)
    vec3 specular = vec3(pow(max(dot(normal, h), 0.0), shininess));
    
    // Compute shadow
    float shadow;
//     #ifdef CSM_DEBUG
//         int shadowDebug;
//     #endif
//     for (int i = 0; i < NUM_CASCADES; i++) {
//         if (proj.z <= -endCascade[i]) {
//             shadow = shadowCalculation(i, lightProj.position[i], normal, s); 
//             #ifdef CSM_DEBUG
//                 shadowDebug = i;
//             #endif
//             break;
//         }
//     }
    if (proj.z <= -endCascade[2]) {
        shadow = shadowCalculation(2, lightProj.position[2], normal, s); 
        if (proj.z <= -endCascade[1]) {
            shadow = shadowCalculation(1, lightProj.position[1], normal, s); 
            if (proj.z <= -endCascade[0]) {
                shadow = shadowCalculation(0, lightProj.position[0], normal, s); 
            }
        }
    }

    // Calculate final color
    vec3 color = lightIntensity * texture(diffuseSampler, texCoordOffset).rgb;
//     #ifdef CSM_DEBUG
//         color[shadowDebug] = 1.0;
//     #endif
    color = color * (
        Ka +                    // Ambient
        (1.0 - shadow) * (
            Kd * diffuse +      // Diffuse
            Ks * specular       // Specular
        )
    );
    
    // Return the fragment color
    fragColor = vec4(color, alpha);
}
