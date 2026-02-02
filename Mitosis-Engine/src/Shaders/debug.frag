#version 330 core

in vec4 debugColor;
out vec4 FragColor;

uniform mat4 viewProj;

void main()
{
	FragColor = debugColor;
}