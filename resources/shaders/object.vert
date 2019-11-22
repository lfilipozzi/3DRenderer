#version 330

const int NUM_CASCADES = 3;     // Number of cascaded shadows

in highp vec3 vertexPosition;
in highp vec3 vertexNormal;
in mediump vec2 texCoord2D;

uniform highp mat4 M;
uniform highp mat4 MV;
uniform highp mat4 MVP;
uniform highp mat4 lMVP[NUM_CASCADES];
uniform highp mat3 N;

out highp vec2 texCoord;
out highp vec3 normal_V;
out highp vec3 position_V;
out highp vec4 position_lP[NUM_CASCADES];
out highp vec3 position_W;
out highp vec3 normal_W;
out highp float zPosition_P;

void main(void) {
    // Compute fragment position and normal in world space 
    // (for environment mapping)
    position_W = vec3(M * vec4(vertexPosition, 1.0));
    normal_W = mat3(transpose(inverse(M))) * vertexNormal;
    
    // Transform to viewSpace
    normal_V = normalize(N * vertexNormal);
    position_V = vec3(MV * highp vec4(vertexPosition, 1.0));
    
    // Transform to light coordinates (for shadow mapping)
    for (int i = 0; i < NUM_CASCADES; i++)
        position_lP[i] = lMVP[i] * highp vec4(vertexPosition, 1.0);
    
    // Pass texture coordinates to the fragment shader
    texCoord = texCoord2D;
    
    gl_Position = MVP * highp vec4(vertexPosition, 1.0);
    
    // Give the z-coordinate in the clip space for cascaded shadow mapping
    zPosition_P = gl_Position.z;
}








