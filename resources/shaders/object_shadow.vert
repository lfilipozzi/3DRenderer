#version 120

// Simple vertex shader used to transform to light space for shadow mapping

attribute highp vec3 vertexPosition;

uniform mat4 lMVP;

void main()
{
    gl_Position = lMVP * vec4(vertexPosition, 1.0);
}  
