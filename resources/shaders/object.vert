#version 330

in highp vec3 vertexPosition;
in highp vec3 vertexNormal;
in mediump vec2 texCoord2D;

uniform highp mat4 M;
uniform highp mat4 MV;
uniform highp mat4 MVP;
uniform highp mat4 lMVP;
uniform highp mat3 N;

out highp vec2 texCoord;
out highp vec3 normal_V;
out highp vec3 position_V;
out highp vec4 fragPos_lP;
out highp vec3 position_W;
out highp vec3 normal_W;

void main(void) {
    // Compute fragment position and normal in world space 
    // (for environment mapping)
    position_W = vec3(M * vec4(vertexPosition, 1.0));
    normal_W = mat3(transpose(inverse(M))) * vertexNormal;
    
    // Transform to viewSpace
    normal_V = normalize(N * vertexNormal);
    position_V = vec3(MV * highp vec4(vertexPosition, 1.0));
    
    // Transform to light coordinates (for shadow mapping)
    fragPos_lP = lMVP * highp vec4(vertexPosition, 1.0);
    
    // Pass texture coordinates to the fragment shader
    texCoord = texCoord2D;
    
    gl_Position = MVP * highp vec4(vertexPosition, 1.0);
}








