#version 330

in highp vec3 vertexPosition;
in highp vec3 vertexNormal;
in mediump vec2 texCoord2D;

uniform highp mat4 M;
uniform highp mat4 MV;
uniform highp mat4 MVP;
uniform highp mat4 lightMVP;
uniform highp mat3 N;

out highp vec2 texCoord;
out highp vec3 normalViewSpace;
out highp vec3 positionViewSpace;
out highp vec4 fragPosLightProjectionSpace;
out highp vec3 positionWorldSpace;
out highp vec3 normalWorldSpace;

void main(void) {
    // Compute fragment position and normal in world space 
    // (for environment mapping)
    positionWorldSpace = vec3(M * vec4(vertexPosition, 1.0));
    normalWorldSpace = mat3(transpose(inverse(M))) * vertexNormal;
    
    // Transform to viewSpace
    normalViewSpace = normalize(N * vertexNormal);
    positionViewSpace = vec3(MV * highp vec4(vertexPosition, 1.0));
    
    // Transform to light coordinates (for shadow mapping)
    fragPosLightProjectionSpace = lightMVP * highp vec4(vertexPosition, 1.0);
    
    // Pass texture coordinates to the fragment shader
    texCoord = texCoord2D;
    
    gl_Position = MVP * highp vec4(vertexPosition, 1.0);
}








