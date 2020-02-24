#version 330 core

const int NUM_CASCADES = 3;     // Number of cascaded shadows

// Light information
uniform vec4 lightDirection;
uniform vec3 lightIntensity;

// Material information
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float shininess;
uniform float alpha;

// Camera information
uniform vec3 cameraPosition;

// Texture sampler
uniform samplerCube skybox;
uniform sampler2D textureSampler;
uniform sampler2D shadowMap[NUM_CASCADES];

// Far plane distance of each shadow cascade
uniform float endCascade[NUM_CASCADES];

in vec2 texCoord;
in vec3 normal_V;
in vec3 position_V;
in vec4 position_lP[NUM_CASCADES];
in vec3 position_W;
in vec3 normal_W;
in float zPosition_P;

// in vec3 TangentLightDir;
// in vec3 TangentViewPos;
// in vec3 TangentFragPos;

in Tangent {
    highp vec3 lightDir;
    highp vec3 viewPos;
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
    float closestDepth = texture(shadowMap[cascadeIndex], projCoords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check if the current fragment is in the shadow
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.0005);
    // Use PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap[cascadeIndex], 0);
    for(int x = -PCF_TEXELSIZE_FILTER; x <= PCF_TEXELSIZE_FILTER; ++x)
    {
        for(int y = -PCF_TEXELSIZE_FILTER; y <= PCF_TEXELSIZE_FILTER; ++y)
        {
            float pcfDepth = texture(shadowMap[cascadeIndex], projCoords.xy + vec2(x, y) 
                * texelSize).r; 
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

// Phong shading model
vec3 adsModel(vec3 norm) {
    // Calculate light direction
//     vec3 s = normalize(-TangentLightDir);
    vec3 s = normalize(-tangent.lightDir);

    // Calculate the vector from the fragment to eye position
//     vec3 v = normalize(-TangentViewPos);
    vec3 v = normalize(-tangent.viewPos);
    
    // Compute the halfway vector for the Blinn-Phong lighting model
    vec3 h = normalize(s + v);
    
    // Calculate the diffuse contribution
    vec3 diffuse = vec3(max(dot(s, norm), 0.0));

    // Calculate specular contribution (Blinn-Phong model)
    vec3 specular = vec3(pow(max(dot(norm, h), 0.0), shininess));
    
    // Compute shadow
    float shadow;
    #ifdef CSM_DEBUG
        int shadowDebug;
    #endif
    for (int i = 0; i < NUM_CASCADES; i++) {
        if (zPosition_P <= -endCascade[i]) { 
            shadow = shadowCalculation(i, position_lP[i], norm, s); 
            #ifdef CSM_DEBUG
                shadowDebug = i;
            #endif
            break;
        }
    }

    // Calculate final color
    vec3 color = lightIntensity * texture(textureSampler, texCoord).rgb;
    #ifdef CSM_DEBUG
        color[shadowDebug] = 1;
    #endif
    return color * (
            Ka +                    // Ambient
            (1.0 - shadow) * (
                Kd * diffuse +      // Diffuse
                Ks * specular       // Specular
                )
        );
}

void main() {
    vec3 normal = vec3(0.5, 0.5, 1.0);
    normal = normalize(normal * 2.0 - 1.0);
    fragColor = vec4(adsModel(normal), alpha);
}
