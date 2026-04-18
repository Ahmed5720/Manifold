#version 410 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
uniform mat4 uProj;
uniform mat4 uView;

out vec3 vNormal;
out vec3 vWorldPos;
void main()
{   
    vWorldPos   = aPos;
    vNormal     = aNormal;
    gl_Position = uProj * uView * vec4(aPos, 1.0);
}

