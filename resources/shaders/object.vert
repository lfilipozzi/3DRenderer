#version 330

const int NUM_CASCADES = 3;     // Number of cascaded shadows

in highp   vec3 vertexPosition;
in highp   vec3 vertexNormal;
in mediump vec2 texCoord2D;
in highp   vec3 vertexTangent;
in highp   vec3 vertexBitangent;

uniform highp mat4 M;
uniform highp mat4 MV;
uniform highp mat4 MVP;
uniform highp mat4 lMVP[NUM_CASCADES];
uniform highp mat3 N;

uniform vec4 lightDirection;

out highp vec2 texCoord;

struct View {
    highp vec3 position;
} view;

out Proj {
    highp float z;
} proj;

out LightProj {
    highp vec4 position[NUM_CASCADES];
} lightProj;

out Tangent {
    highp vec3 lightDir;
    highp vec3 fragPos;
} tangent;



void main(void) {    
    // Pass texture coordinates to the fragment shader
    texCoord = texCoord2D;
    
    // Transform to the vertex position to view space
    view.position = vec3(MV * highp vec4(vertexPosition, 1.0));
    
    // Transform to light space (for shadow mapping)
//     for (int i = 0; i < NUM_CASCADES; i++)
//         lightProj.position[i] = lMVP[i] * highp vec4(vertexPosition, 1.0);
    lightProj.position[0] = lMVP[0] * highp vec4(vertexPosition, 1.0);
    lightProj.position[1] = lMVP[1] * highp vec4(vertexPosition, 1.0);
    lightProj.position[2] = lMVP[2] * highp vec4(vertexPosition, 1.0);
    
    // Transform the vertex position to clip space
    gl_Position = MVP * highp vec4(vertexPosition, 1.0);
    
    // Give the z-coordinate in the clip space for cascaded shadow mapping
    proj.z = gl_Position.z;
    
    // Compute TBN matrix
    vec3 Tvec = normalize(N * vertexTangent);
    // vec3 Bvec = normalize(N * vertexBitangent);
    vec3 Nvec = normalize(N * vertexNormal);
    vec3 Bvec = cross(Nvec,Tvec);
    mat3 TBN = transpose(mat3(Tvec, Bvec, Nvec));
    
    // Transform from view space to tangent space
    tangent.lightDir  = TBN * lightDirection.xyz;
    tangent.fragPos   = TBN * view.position.xyz;
}








