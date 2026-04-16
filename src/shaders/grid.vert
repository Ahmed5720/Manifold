#version 410 core

layout(location = 0) in vec3 aPos;

uniform mat4 uProj;
uniform mat4 uView;

out vec3 vColor;
void main()
{
    gl_Position = uProj * uView * vec4(aPos, 1.0);
    vColor = vec3(1.0);
}