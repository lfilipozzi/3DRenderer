#version 330 core

// Arrow color
uniform vec3 color;

layout (location = 0) out vec4 fragColor;

void main()
{
    fragColor = vec4(color, 1.0);
}
