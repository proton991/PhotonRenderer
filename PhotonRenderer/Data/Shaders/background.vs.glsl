#version 450 core

layout (location = 0) in vec3 aPos;

out vec3 vTexCoords;

uniform mat4 uProjView;

void main()
{
    vTexCoords = aPos;
    vec4 pos = uProjView * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}