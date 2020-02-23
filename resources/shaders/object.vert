#version 330

const int NUM_CASCADES = 3;     // Number of cascaded shadows

in highp vec3 vertexPosition;
in highp vec3 vertexNormal;
in mediump vec2 texCoord2D;
in highp vec3 vertexTangent;
in highp vec3 vertexBitangent;

uniform highp mat4 M;
uniform highp mat4 MV;
uniform highp mat4 MVP;
uniform highp mat4 lMVP[NUM_CASCADES];
uniform highp mat3 N;

uniform vec4 lightDirection;

out highp vec2 texCoord;
out highp vec3 normal_V;
out highp vec3 position_V;
out highp vec4 position_lP[NUM_CASCADES];
out highp vec3 position_W;
out highp vec3 normal_W;
out highp float zPosition_P;

out highp vec3 TangentLightDir;
out highp vec3 TangentViewPos;
out highp vec3 TangentFragPos;

out highp mat3 TBN;

out vec3 test;

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
    
    // Compute TBN matrix
    vec3 T    = normalize(N * vertexTangent);
//     vec3 B    = normalize(N * vertexBitangent);
    vec3 Nvec = normalize(N * vertexNormal);
//     vec3 T    = N * vertexTangent;
//     vec3 B    = N * vertexBitangent;
//     vec3 Nvec = N * vertexNormal;
//     vec3 T    = normalize(mat3(MV) * vertexTangent);
//     vec3 B    = normalize(mat3(MV) * vertexBitangent);
//     vec3 Nvec = normalize(mat3(MV) * vertexNormal);
    vec3 B = cross(Nvec,T);
//     vec3 T = cross(B,Nvec);
    TBN = transpose(mat3(T, B, Nvec));
// mat3 TBN = mat3(T, B, N);
    
    TangentLightDir = TBN * lightDirection.xyz;
    TangentViewPos  = TBN * position_V.xyz;
    TangentFragPos  = TBN * position_W;
    
    test = 0.5*(normalize(T)+vec3(1.0,1.0,1.0));
}








