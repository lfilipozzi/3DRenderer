#version 120

attribute highp vec3 vertexPosition;

uniform highp mat4 MVP;

void main(void)
{
    gl_Position = MVP * highp vec4(vertexPosition, 1.0);
}
