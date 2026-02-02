#version 330 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inColor;

uniform mat4 viewProj;

out vec4 debugColor;

void main()
{
	debugColor = inColor;
	gl_Position = viewProj * vec4(inPos.xy, 0.0, 1.0);
}