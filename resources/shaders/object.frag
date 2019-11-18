#version 330 core

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
uniform sampler2D shadowMap;

in vec2 texCoord;
in vec3 normal_V;
in vec3 position_V;
in vec4 fragPos_lP;
in vec3 position_W;
in vec3 normal_W;

out vec4 fragColor;

// Compute shadow with shadow mapping
#define PCF_TEXELSIZE_FILTER 2
float shadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    // Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range 
    // fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.0005);
    // Use PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -PCF_TEXELSIZE_FILTER; x <= PCF_TEXELSIZE_FILTER; ++x)
    {
        for(int y = -PCF_TEXELSIZE_FILTER; y <= PCF_TEXELSIZE_FILTER; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) 
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
    vec3 s = normalize(-lightDirection.xyz);

    // Calculate the vector from the fragment to eye position
    vec3 v = normalize(-position_V.xyz);
    
    // Compute the halfway vector for the Blinn-Phong lighting model
    vec3 h = normalize(s + v);
    
    // Calculate the diffuse contribution
    vec3 diffuse = vec3(max(dot(s, norm), 0.0));

    // Calculate specular contribution (Blinn-Phong model)
    vec3 specular = vec3(pow(max(dot(norm, h), 0.0), shininess));

    // Calculate final color
    vec3 color = lightIntensity * texture(textureSampler, texCoord).rgb;
    float shadow = shadowCalculation(fragPos_lP, norm, s); 
    return color * (
            Ka +                            // Ambient
            (1.0 - shadow) * (
                Kd * diffuse +     // Diffuse
                Ks * specular)     // Specular
        );
}

vec3 refraction(vec3 position, vec3 normal) {
    vec3 incidentRay = normalize(position - cameraPosition);
    vec3 reflectionRay = reflect(incidentRay, -normalize(normal));
    return texture(skybox, reflectionRay).rgb;
}

void main() {
    fragColor = vec4(adsModel(normalize(normal_V)), alpha);
}
