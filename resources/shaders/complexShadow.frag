#version 330 core

// Empty fragment shader used for shadow mapping

void main()
{             
    gl_FragDepth = gl_FragCoord.z;
}  
